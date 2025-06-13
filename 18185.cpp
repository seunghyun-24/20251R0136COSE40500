#include <iostream>
#include <algorithm>
#include <vector>

using ll = long long int;
using pii = std::pair<int, int>;
using pll = std::pair<long long int, long long int>;

#define all(x) (x).begin(), (x).end()

using namespace std;
void init();

int main() {
    init();
    int n; cin >> n; vector<int> v(n);
    for (int i = 0; i < n; i++) { cin >> v[i]; }

    int idx = 0, ans = 0;
    while (idx < n) {
        if (v[idx] > 0) {
            int temp = v[idx];
            ans += 3 * temp;
            if (idx + 1 < n) {
                temp = min(temp, v[idx+1]);
                ans += 2 * temp;
                v[idx+1] -= temp;
                if (temp && idx + 2 < n) {
                    temp = min(temp, v[idx+2]-min(v[idx+1], v[idx+2]));
                    ans += 2 * temp;
                    v[idx+2] -= temp;
                }
            }
        }
        ++idx;
    }
    cout << ans;

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