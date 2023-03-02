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

const int N = 310, M = 5e6+10;
const ll INF = 0x3f3f3f3f3f3f3f3f;
const ll mod = 998244353;
const double PI = acos(-1.0);

int n;
int a[N];

int main() {
    IOS;
	cin>>n;
	for(int i=1;i<=n;i++)cin>>a[i];
	vector<ll> dp(2*n*N,0);
	int oft=n*300;
	dp[a[2]+oft]=1;
	for(int i=2;i<n;i++){
		vector<ll>ndp(2*n*N,0);
		for(int j=-300*n;j<=n*300;j++){
			if(j==0){
				ndp[j+oft+a[i+1]]+=dp[j+oft];
				ndp[j+oft+a[i+1]]%=mod;
			}else{
				ll a1=a[i+1]+oft+j;
				ll a2=a[i+1]-j+oft;
				if(a1<=2*n*300&&a1>=0){
					ndp[a1]+=dp[j+oft];
					ndp[a1]%=mod;
				}
				if(a2<=2*n*300&&a2>=0){
					ndp[a2]+=dp[j+oft];
					ndp[a2]%=mod;
				}
			}	
		}
		dp=ndp;
	}
	ll ans=0;
	for(int i=0;i<=2*n*300;i++){
		ans=(ans+dp[i])%mod;
	}
	cout<<ans<<endl;
    return 0;
}