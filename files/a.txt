#include <bits/stdc++.h>
using namespace std;
//freopen("input.txt", "r", stdin);
//freopen("output.txt", "w", stdout);
typedef long long ll;
typedef unsigned long long ull;
#define FIN ios::sync_with_stdio(0);cin.tie(0);cout.tie(0)
#define forr(i, a, b) for(ll i = (a); i < (ll) (b); i++)
#define forn(i, n) forr(i, 0, n)
#define pb push_back
#define mp make_pair
#define all(c) (c).begin(),(c).end()
#define DBG(x) cerr << #x << " = " << (x) << endl
#define MOD 1000000007

int bindig(ll num)
{
 int digitos=0;
 while(num > 0)
    {
        num=num/2;
        digitos++;
    }
 return digitos;

}

int main() {
    FIN;
    ll q;
    cin >> q;
    forn(j,q)
    {
        ll d, m;
        cin >> d >> m;
        ll miresp = bindig(d);
        ll aux = pow((ll)2,miresp-1);
        ll ter = d - aux + 1;
        ll ans2=1; ans2%=m;
        forr(i,2,miresp+1)
        {
        if(i==miresp)
        {
            ans2+=ans2*ter+ter;
            ans2%=m;
            continue;
        }
        ans2+=ans2*(pow(2,i-1))+(pow(2,i-1));
        ans2%=m;
        }
        cout << ans2 << endl;
    }

    return 0;
}