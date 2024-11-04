#include <bits/stdc++.h>
using namespace std;
//freopen("input.txt", "r", stdin);
//freopen("output.txt", "w", stdout);
 
typedef long long ll;
#define FIN ios::sync_with_stdio(0);cin.tie(0);cout.tie(0)
#define forr(i, a, b) for(int i = (a); i < (int) (b); i++)
#define forn(i, n) forr(i, 0, n)
#define pb push_back
#define mp make_pair
#define all(c) (c).begin(),(c).end()
#define DBG(x) cerr << #x << " = " << (x) << endl
#define DBGV(v,n) forn(i,n) cout << v[i] << " "; cout << endl
#define esta(x,c) ((c).find(x) != (c).end())
#define RAYA cerr << "===============================" << endl


ll funcion(ll d)
{
	ll cont=0;
	while(d > 0) {d/=2; cont++;}
	return cont;
	
}

int main()
{ 
	FIN;
	
	#ifdef input
		freopen("test1.txt", "r", stdin);
	#endif
	
	int t;
	cin >> t;
	while(t--)
	{
		ll d, m;
		cin >> d >> m;
		ll cont = funcion(d);
		ll aux = 1<<(cont-1);
		ll fin = d - aux + 1;
		ll ans=1; ans%=m;
		//DBG(cont); DBG(ans);
		for(int i = 2; i<=cont; i++)
		{
			if(i==cont)
			{
				ans+=ans*fin+fin;
				ans%=m;
				continue;
			}
			ans+=ans*(1<<(i-1))+(1<<(i-1));
			ans%=m;
		}
		cout << ans << endl;
	}
	return 0;
}