#include <bits/stdc++.h>
#define rson rt << 1 | 1
#define lson rt << 1
#define pb push_back
#define endl '\n'
#define x first
#define y second
#define double long double
#define LLINF 9223372036854775807
#define IOS                  \
    ios::sync_with_stdio(0); \
    cin.tie(0);
using namespace std;
typedef long long ll;
typedef unsigned long long ull;
typedef pair<int, int> pii;
typedef pair<ll, ll> pll;
typedef pair<double, double> pdd;
typedef tuple<int, int, int> tp;

const int N = 1e4 + 10, M = 2e7 + 10;
const ll INF = 0x3f3f3f3f3f3f3f3f;
const ll mod = 998244353;
const double PI = acos(-1.0);

int n, m, S, T;
int h[N], e[M], ne[M], idx, cur[N];
int dep[N];
ll w[M];
int c;
bool st[2 * N];
vector<int> g[N];
bool vis[N];

void add(int a, int b, ll c) {
    e[idx] = b, ne[idx] = h[a], w[idx] = c, h[a] = idx++;
    e[idx] = a, ne[idx] = h[b], w[idx] = 0, h[b] = idx++;
}

bool bfs() {
    for (int i = 0; i <= n + 2; i++) dep[i] = 0, cur[i] = h[i];
    dep[S] = 1;
    queue<int> q;
    q.push(S);
    while (q.size()) {
        int t = q.front();
        q.pop();
        for (int i = h[t]; i != -1; i = ne[i]) {
            int j = e[i];
            if (!dep[j] && w[i] > 0) {
                dep[j] = dep[t] + 1;
                q.push(j);
            }
        }
    }
    return dep[T];
}

ll dfs(int x, ll flow) {
    if (x == T) {
        return flow;
    }
    ll res = 0;
    for (int& i = cur[x]; i != -1; i = ne[i]) {
        int j = e[i];
        if (dep[j] != dep[x] + 1 || !w[i]) continue;
        ll tp = dfs(j, min(flow - res, w[i]));
        w[i] -= tp;
        w[i ^ 1] += tp;
        res += tp;
        if (res == flow) break;
    }
    if (!res) dep[x] = 0;
    return res;
}

ll Dinic() {
    ll ans = 0;
    while (bfs()) {
        ans += dfs(S, INF);
    }
    return ans;
}


int main() {
    IOS;

    return 0;
}