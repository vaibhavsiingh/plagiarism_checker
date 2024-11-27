#include <bits/stdc++.h>
using namespace std;

typedef int64_t ll;
typedef vector<ll> vll;
typedef pair<ll, ll> pll;

const ll MOD = 1000000007;

vector<pll> directions = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}}; // Up, Right, Down, Left

// Function to perform BFS and calculate minimum distance
ll bfs(const vector<vector<char>>& grid, vector<vll>& dist, pll start, ll n, ll m) {
    queue<pll> q;
    q.push(start);
    dist[start.first][start.second] = 0;

    while (!q.empty()) {
        ll x = q.front().first, y = q.front().second;
        q.pop();

        for (const pll& dir : directions) {
            ll nx = x + dir.first, ny = y + dir.second;
            if (nx >= 0 && nx < n && ny >= 0 && ny < m && dist[nx][ny] == -1 && grid[nx][ny] != '#') {
                dist[nx][ny] = dist[x][y] + 1;
                if (grid[nx][ny] == 'B') {
                    return dist[nx][ny]; // Early exit if we reach 'B'
                }
                q.push({nx, ny});
            }
        }
    }
    return -1; // Return -1 if no path exists
}

// Function to trace the path from end to start
string tracePath(const vector<vll>& dist, pll end, ll n, ll m) {
    string path;
    ll x = end.first, y = end.second;

    while (dist[x][y] != 0) {
        for (const pll& dir : directions) {
            ll nx = x + dir.first, ny = y + dir.second;
            if (nx >= 0 && nx < n && ny >= 0 && ny < m && dist[nx][ny] == dist[x][y] - 1) {
                x = nx;
                y = ny;
                if (dir == directions[0]) path.push_back('D'); // Up
                else if (dir == directions[1]) path.push_back('L'); // Right
                else if (dir == directions[2]) path.push_back('U'); // Down
                else path.push_back('R'); // Left
                break;
            }
        }
    }
    reverse(path.begin(), path.end()); // Reverse the path to get correct order
    return path;
}

void solve() {
    ll n, m;
    cin >> n >> m;
    
    vector<vector<char>> grid(n, vector<char>(m));
    pll start, end;
    
    // Input the grid and find start ('A') and end ('B')
    for (ll i = 0; i < n; ++i) {
        for (ll j = 0; j < m; ++j) {
            cin >> grid[i][j];
            if (grid[i][j] == 'A') start = {i, j};
            if (grid[i][j] == 'B') end = {i, j};
        }
    }

    // Initialize distance grid with -1
    vector<vll> dist(n, vll(m, -1));
    
    // Perform BFS to calculate the shortest distance
    ll pathLength = bfs(grid, dist, start, n, m);

    if (pathLength != -1) {
        cout << "YES\n" << pathLength << '\n';
        cout << tracePath(dist, end, n, m) << '\n'; // Output the path
    } else {
        cout << "NO\n";
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    solve();
    return 0;
}
