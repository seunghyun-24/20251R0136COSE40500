#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <cmath>

using ll = long long int;
using pii = std::pair<int, int>;
using pll = std::pair<long long int, long long int>;

#define all(x) (x).begin(), (x).end()
using namespace std;
void init();

int n, m, k;
int cntChess(char color, const vector<vector<char>>& chess ) {
    int value;
    vector<vector<int>> sum(n+1, vector<int>(m+1, 0));

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if ( ((i+j)%2) ) value = (chess[i][j] == color) ? 1 : 0;
            else value = (chess[i][j] == color) ? 0 : 1;

            sum[i+1][j+1] = sum[i+1][j] + sum[i][j+1] - sum[i][j] + value;
        }
    }

    int tmp, result = k*k;

    for (int i = 0; i <= n-k; ++i) {
        for (int j = 0; j <= m-k; ++j) {
            tmp = sum[i+k][j+k] - sum[i+k][j] - sum[i][j+k] + sum[i][j];
            result = min(result, tmp);
        }
    }

    return result;
}

int main() {
    init();

    cin >> n >> m >> k;

    vector<vector<char>> chess(n, vector<char>(m));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            cin >> chess[i][j];
        }
    }

    cout << min(cntChess('W', chess), cntChess('B', chess));



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