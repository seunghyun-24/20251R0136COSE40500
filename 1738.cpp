#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <stack>
#include <climits>
#include <queue>

using ll = long long int;
using pii = std::pair<int, int>;
using pll = std::pair<long long int, long long int>;

#define all(x) (x).begin(), (x).end()
const long long LLINF = 1e18;

using namespace std;
void init();

int main() {
    init();

    ll n, m; cin >> n >> m;
    vector<vector<pll>> adj(n+1);
    vector<vector<pll>> rev_adj(n+1);
    vector<bool> visited(n+1, false);

    for(int i = 0; i < m; ++i) {
        int u, v, w;
        cin >> u >> v >> w;
        adj[u].push_back({v, -w});
        rev_adj[v].push_back({u, -w});
    }

    queue<ll> q;
    q.push(n);
    visited[n] = true;
    while(!q.empty()) {
        ll u = q.front();
        q.pop();
        for(auto &i: rev_adj[u]) {
            ll v = i.first;
            if(visited[v] == 0) {
                q.push(v);
                visited[v] = true;
            }
        }
    }

    if(visited[1] == 0) {
        cout << -1;
        return 0;
    }

    vector<ll> ans(n+1, LLINF);
    ans[1] = 0;

    vector<ll> tmp(n+1, -1);
    for(ll i = 1; i <= n; ++i) {
        for(ll j = 1; j <= n; ++j) {
            for(auto& a: adj[j]) {
                ll v = a.first;
                ll w = a.second;
                if(ans[j] != LLINF && ans[v] > ans[j] + w) {
                    tmp[v] = j;
                    ans[v] = ans[j] + w;

                    if((i == n) && visited[j]) {
                        cout << -1;
                        return 0;
                    }
                }
            }
        }
    }


    vector<ll> path;
    for(ll cur = n; cur != -1; cur = tmp[cur]) {
        path.push_back(cur);
    }
    reverse(all(path));

    for(auto x : path) {
        cout << x << ' ';
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