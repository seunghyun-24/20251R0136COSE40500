#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>
#include <queue>
#include <map>

using ll = long long int;
using pii = std::pair<int, int>;
using pll = std::pair<long long int, long long int>;

#define all(x) (x).begin(), (x).end()

using namespace std;
void init();

int mv[5] = {0, 1, 0, -1, 0};

int main() {
    init();
    int t, h, w; string key; cin >> t;
    while(t--) {
        cin >> h >> w;
        vector<vector<char>> state(h, vector<char>(w));
        queue<pii> q;
        for(int i = 0; i < h; i++) {
            for(int j = 0; j < w; j++) {
                cin >> state[i][j];
                if((i == 0 || j == 0 || i == h-1 || j == w-1) && state[i][j] != '*') q.push({i, j});
            }
        }
        cin >> key;
        vector<bool> keyMap(26, false);
        if(key != "0") {
            for(int i = 0; i < key.length(); i++) {
               keyMap[key[i]-97] = true;
            }
        }
        vector<vector<bool>> visited(h, vector<bool>(w, false));
        vector<queue<pii>> doors(26);
        int cnt = 0;
        while(!q.empty()) {
            pii p = q.front();
            q.pop();
            if(visited[p.first][p.second]) continue;

            if(isupper(state[p.first][p.second])){ // 문이면은 열쇠가 있어야 갈 수 있다.
                if(!keyMap[state[p.first][p.second]-65]) {
                    doors[state[p.first][p.second]-65].push(p);
                    continue; // 열쇠 없는 케이스라 못감
                }
                // 그 외는 열쇠가 있는 케이스인 셈
            } else if(islower(state[p.first][p.second])) { // 열쇠면 먹자
                if(!keyMap[state[p.first][p.second]-97]) {
                    keyMap[state[p.first][p.second]-97] = true;
                    while(!doors[state[p.first][p.second]-97].empty()) {
                        q.push(doors[state[p.first][p.second]-97].front());
                        doors[state[p.first][p.second]-97].pop();
                    }
                }
            } else if(state[p.first][p.second] == '$') { // 문서
                ++cnt;
            }

            visited[p.first][p.second] = true; // 방문 체크하고
            state[p.first][p.second] = '.'; // 따잇 !

            for(int m = 0; m < 4; ++m) {
                int nx = p.first + mv[m];
                int ny = p.second + mv[m+1];
                if(nx < 0 || nx >= h || ny < 0 || ny >= w) continue;
                if(state[nx][ny] == '*' || visited[nx][ny]) continue;
                q.push({nx, ny});
            }
        }
        cout << cnt << '\n';
    }

    return 0;
}

void init() {
#ifndef ONLINE_JUDGE
    freopen("input.txt", "rt", stdin);
    freopen("output.txt", "wt", stdout);
#endif
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    cout.tie(NULL);
}