#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <linux/types.h>
#include <linux/netfilter.h>		/* for NF_ACCEPT */
#include <errno.h>
#include <iso686.h>

#include <libnetfilter_queue/libnetfilter_queue.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <string.h>

char* blocked; //일단 혹시 모르니까.. 뭔가 이상함. 엥.. ha..

bool parseHTTP(char* payload) {
	
	//Host: 를 찾고... \r\n
	const char* method = "Host: "
	char* ptr = strtok(payload, "\r\n");
	if(ptr == NULL) return false;

	ptr = strtok(NULL, "\r\n");
	
	if(ptr == NULL) return false;
	
	ptr = strstr(ptr, method);
	
	if(ptr == NULL) return false;
	
	if(strcmp(ptr, blocked) != 0) return false;
	
	return true;
}

bool check(unsigned char* data) {
	
    struct ip* ipHeader;
	struct tcphdr* tcpHeader;

	uint32_t ipLen, tcpOffSet;

	ipHeader = (struct ip*)(data);
	ipLen = (ipHeader->ip_hl)*4;

	if (ipHeader->ip_p != IPPROTO_TCP)
		//printf("Not TCP\n");
		puts("Not TCP");
	else {

		tcpHeader = (struct tcphdr*)(data + ipLen);
		
		//check http
		if (ntohl(tcpHeader->th_dport) != 80 && ntohl(tcpHeader->th_sport) != 80) {
			//printf("Not HTTP\n");
			puts("Not HTTP");
			return false;
		}

		tcpOffSet = (tcpHeader->th_off)*4;

		char* payload = (char *)(data + ipLen + tcpOffSet);
		//안에서 찾아낸 값이랑 data랑 동일하면 true 반환
		
		if(parseHTTP(payload)) return true;
		else false;
    }
    return false;
}


/* returns packet id */
static u_int32_t print_pkt (struct nfq_data *tb)
{
	int id = 0;
	struct nfqnl_msg_packet_hdr *ph;
	struct nfqnl_msg_packet_hw *hwph;
	u_int32_t mark,ifi;
	int ret;
	unsigned char *data;

	ph = nfq_get_msg_packet_hdr(tb);
	if (ph) {
		id = ntohl(ph->packet_id);
		printf("hw_protocol=0x%04x hook=%u id=%u\n",
			ntohs(ph->hw_protocol), ph->hook, id);
	}

	hwph = nfq_get_packet_hw(tb);
	if (hwph) {
		int i, hlen = ntohs(hwph->hw_addrlen);

		printf("hw_src_addr=");
		for (i = 0; i < hlen-1; i++)
			printf("%02x:", hwph->hw_addr[i]);
		printf("%02x\n", hwph->hw_addr[hlen-1]);
	}

	mark = nfq_get_nfmark(tb);
	if (mark)
		printf("mark=%u\n", mark);

	ifi = nfq_get_indev(tb);
	if (ifi)
		printf("indev=%u\n", ifi);

	ifi = nfq_get_outdev(tb);
	if (ifi)
		printf("outdev=%u\n", ifi);
	ifi = nfq_get_physindev(tb);
	if (ifi)
		printf("physindev=%u\n", ifi);

	ifi = nfq_get_physoutdev(tb);
	if (ifi)
		printf("physoutdev=%u\n", ifi);

	ret = nfq_get_payload(tb, &data);
	if (ret >= 0) {
		printf("payload_len=%d\n", ret);
	}

	fputc('\n', stdout);

	return id;
}

static u_int32_t findingID(struct nfq_data *tb) {
	struct nfqnl_msg_packet_hdr *ph;
	ph = nfq_get_msg_packet_hdr(tb);
	
	if (ph) return ntohl(ph->packet_id);
	else return -1;
}

static int cb(struct nfq_q_handle *qh, struct nfgenmsg *nfmsg,
	      struct nfq_data *nfa, void *data)
{
	//u_int32_t id = print_pkt(nfa);
	//printf("entering callback\n");
	
	findingID(nfa);
	
	if (check(data)) return nfq_set_verdict(qh, id, NF_DROP, 0, NULL);
    	else return nfq_set_verdict(qh, id, NF_ACCEPT, 0, NULL);
	
}

int main(int argc, char **argv)
{
	if (2 != argc) {
		printf("syntax : syntax : netfilter-test <host>\n");
        	printf("sample : sample : netfilter-test test.gilgil.net\n");
		return -1;
	}

	//char* blocked = (char*)(argv[1]);
	blocked = (char*)(argv[1]);

    struct nfq_handle *h;
	struct nfq_q_handle *qh;
	struct nfnl_handle *nh;
	int fd;
	int rv;
	char buf[4096] __attribute__ ((aligned));

	printf("opening library handle\n");
	h = nfq_open();
	if (!h) {
		fprintf(stderr, "error during nfq_open()\n");
		exit(1);
	}

	printf("unbinding existing nf_queue handler for AF_INET (if any)\n");
	if (nfq_unbind_pf(h, AF_INET) < 0) {
		fprintf(stderr, "error during nfq_unbind_pf()\n");
		exit(1);
	}

	printf("binding nfnetlink_queue as nf_queue handler for AF_INET\n");
	if (nfq_bind_pf(h, AF_INET) < 0) {
		fprintf(stderr, "error during nfq_bind_pf()\n");
		exit(1);
	}

	printf("binding this socket to queue '0'\n");
	qh = nfq_create_queue(h,  0, &cb, NULL);
	if (!qh) {
		fprintf(stderr, "error during nfq_create_queue()\n");
		exit(1);
	}

	printf("setting copy_packet mode\n");
	if (nfq_set_mode(qh, NFQNL_COPY_PACKET, 0xffff) < 0) {
		fprintf(stderr, "can't set packet_copy mode\n");
		exit(1);
	}

	fd = nfq_fd(h);

	for (;;) {

        if ((rv = recv(fd, buf, sizeof(buf), 0)) >= 0) {
			printf("pkt received\n");
			nfq_handle_packet(h, buf, rv);
			continue;
		} 
        if (rv < 0 && errno == ENOBUFS) {
			printf("losing packets!\n");
			continue;
		}

		perror("recv failed\n");
		break;
	}

	printf("unbinding from queue 0\n");
	nfq_destroy_queue(qh);

#ifdef INSANE
	/* normally, applications SHOULD NOT issue this command, since
	 * it detaches other programs/sockets from AF_INET, too ! */
	printf("unbinding from AF_INET\n");
	nfq_unbind_pf(h, AF_INET);
#endif

	printf("closing library handle\n");
	nfq_close(h);

	exit(0);
}