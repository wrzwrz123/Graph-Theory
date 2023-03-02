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

const int N = 1e5+10, M = 5e6+10;
const ll INF = 0x3f3f3f3f3f3f3f3f;
const ll mod = 998244353;
const double PI = acos(-1.0);

int n,m,S,T;
int h[N],e[M],ne[M],cur[N],idx;
ll w[M],fw[M],dist[N];
ll cost,maxflow;
bool st[N];
int a[310][310];
int id[310][310];

void add(int a,int b,int c,int d){
    e[idx]=b,ne[idx]=h[a],fw[idx]=c,w[idx]=d,h[a]=idx++;
    e[idx]=a,ne[idx]=h[b],fw[idx]=0,w[idx]=-d,h[b]=idx++;
}

bool spfa(){
    for(int i=0;i<=2*n*n+2;i++)cur[i]=h[i],dist[i]=-INF,st[i]=0;
    dist[S]=0;
    queue<int>q;
    q.push(S);
    while(q.size()){
        int t=q.front();q.pop();
        st[t]=0;
        for(int i=h[t];i!=-1;i=ne[i]){
            int j=e[i];
            if(fw[i]&&dist[j]<dist[t]+w[i]){
                dist[j]=dist[t]+w[i];
                if(!st[j]){
                    q.push(j);
                    st[j]=1;
                }
            }
        }
    }
    return dist[T]!=-INF;
}

ll dfs(int x,ll flow){
    if(x==T){
        maxflow+=flow;
        return flow;
    }
    if(!flow)return flow;
    st[x]=1;
    ll ans=0;
    for(int i=cur[x];i!=-1;i=ne[i]){
        int j=e[i];
        cur[x]=i;
        if(fw[i]&&!st[j]&&dist[j]==dist[x]+w[i]){
            ll mi=dfs(j,min(flow-ans,fw[i]));
            if(mi){
                ans+=mi;
                fw[i]-=mi;
                fw[i^1]+=mi;
                cost+=mi*w[i];
            }
            if(ans==flow)break;
        }
    }
    st[x]=0;
    if(!ans)st[x]=1;
    return ans;
}

void mincostmaxflow(){
    while(spfa()){
        while(dfs(S,INF));
    }
}


int main() {
    IOS;
    return 0;
}