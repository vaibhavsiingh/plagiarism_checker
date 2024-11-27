#include <bits/stdc++.h>
using namespace std;
 
typedef int64_t ll;
typedef vector<ll> vll;
typedef pair<ll, ll> pll;
 
#define pb push_back
#define pob pop_back
#define ff first
#define ss second
#define sz size
#define reach cout << "Reached" << endl;
#define o1(a) cout << a << "\n"
#define o2(a, b) cout << a << ' ' << b << "\n";
#define o3(a,b,c) cout << a << ' ' << b << ' ' << c  <<"\n";
#define iArray(a, n) for (ll i = 0; i < n; i++) cin >> a[i];
#define i2(a, b) cin >> a >> b;
 
const ll MOD = 1000000007;
 
ll bfs(vector<vector<char>>grid,vector<vll>& dist,pll start,ll n,ll m){
    vector<pll> direc = {{-1,0},{0,1},{1,0},{0,-1}};
    queue<pll> q;
    q.push(start);
    while(!q.empty()){
        ll a =q.front().ff, b=q.front().ss;
        
        q.pop();
        for(pll c:direc){
            ll na = a+c.ff,nb=b+c.ss;
            if(na >=0 && na <n && nb>=0 && nb<m ){
                
                if(grid[na][nb] == 'B') return dist[a][b]+1;
                if(grid[na][nb] == '#') continue;
                if(dist[na][nb]!=-1) continue;
                q.push({na,nb});
                dist[na][nb] = dist[a][b]+1;
            }
        }
    }
    return -1;
    
}
 
void sol(){
    ll n,m;
    i2(n,m);
    pll start,end;
    vector<vector<char>> grid(n,vector<char>(m));
    for(ll i = 0; i < n; i++) {
        for(ll j=0; j<m; j++){
             cin >> grid[i][j];
             if(grid[i][j]=='A') start = {i,j};
             else if(grid[i][j]=='B') end = {i,j};
        }
    }
    vector<vll> dist(n,vll(m,-1));
    dist[start.ff][start.ss] = 0;
    ll length = bfs(grid, dist, start, n,m);
    if(length >0){
        o1("YES");
        o1(length);
        ll i=end.ff,j=end.ss;
        vector<pll> direc = {{-1,0},{0,1},{1,0},{0,-1}};
        vector<char> ans;
        for(ll dis = length-1; dis>=0; dis--){
            for(pll c:direc){
                ll na = i+c.ff,nb=c.ss+j;
                if(na >=0 && na <n && nb>=0 && nb<m ){
                    if(dist[na][nb]==dis){
                        i = na;
                        j = nb;
                        if(c.ff == -1) ans.pb('D');
                        else if(c.ff == 1) ans.pb('U');
                        else if(c.ss == 1) ans.pb('L');
                        else ans.pb('R');
                    }
                }
 
            }
        }
        for(ll i = length-1; i >=0 ; i--) {
            cout << ans[i];
        }
    }
    else o1("NO");
}
 
 
int main(){
    std::ios::sync_with_stdio(false);
    cin.tie(NULL);
    cout.tie(NULL);
    sol();
}