#include <iostream>
#include <algorithm>
#include <vector>
#include <cstdlib>  // for rand, srand
#include <ctime>    // for time
using namespace std;

const int INF = 0x7fffffff;
const int MAXN = 1005;
int origin[MAXN][MAXN]; //原始图的邻接矩阵
int adj[MAXN][MAXN]; //邻接矩阵
int W[MAXN]; //W[i]是邻接矩阵第i行的非重复权值和
int flag[MAXN]; //标记该点是否被合并  
int n, m;

void init()
{
    for(int i = 1; i <= n; ++i)
    {
        for(int j = 1; j <= n; ++j)
        {
            origin[i][j] = 0;
        }
    }
}

void addEdge(int u, int v)
{
    origin[u][v]++;
    origin[v][u]++;
}

void ReGraph()
{
    for(int i = 1; i <= n; ++i)
    {
        flag[i] = 0;  // 重置flag[i]，确保每次重新计算
    }
    for(int i = 1; i <= n; ++i)
    {
        W[i] = 0;  // 重置W[i]，确保每次重新计算
        for(int j = 1; j <= n; ++j)
        {
            adj[i][j] = origin[i][j];
            if (adj[i][j] > 0 && j >= i)  // 只有存在边时才更新W
            {
                W[i] += adj[i][j];
            }
        }
    }
}

void contract(int u, int v)
{
    // 合并节点v到节点u
    for(int i = 1; i <= n; ++i)
    {
        adj[u][i] += adj[v][i];
        adj[i][u] += adj[i][v];
    }
    adj[u][u] = 0;  // 自环边为0
    flag[v] = 1;  // 标记v已经被合并
    
    for(int i = 1; i <= n; ++i)
    {
        if(i < u && v < i && flag[i] == 0)  // 只有i在u和v之间的行才需要更新W[i]
        {
            W[i] += adj[i][v];
        }
        else if(i > u && v > i && flag[i] == 0)  // 只有i在u和v之间的行才需要更新W[i]
        {
            W[i] -= adj[i][v];
        }
    }
    W[u] = 0;  // 单独计算W[u]
    for(int i = u + 1; i <= n; ++i)
    {
        if(flag[i] == 1) continue;
        W[u] += adj[u][i]; 
    }
}

pair<int, int> RandomPick()
{
    int sum = 0;
    int first = INF;
    for(int i = 1; i <= n; ++i)
    {
        if(flag[i] == 1) continue;;
        sum += W[i];
        if (first == INF) first = i;
    }
    
    int target = rand() % sum + 1; // 随机选一条边
    int i = first, w = W[first];
    while (target > w)  // 找到目标区间对应的i
    {
        ++i;
        if(flag[i] == 1) continue;
        w += W[i];
    }
  
    target = target - (w - W[i]);
    int j = i;
    while (target > 0)
    {
        ++j;
        if (flag[j] == 1) continue;
        target -= adj[i][j];
    }
    cout <<"test:"<<i<<" "<<j<<endl;
    return {i, j};
}

int Karger()
{
    ReGraph();
    for (int i = n; i > 2; --i)
    {
        pair<int, int> edge = RandomPick();
        contract(edge.first, edge.second);
    }

    int ans = 0;
    for (int i = 1; i <= n; ++i)
    {
        for (int j = i + 1; j <= n; ++j)
        {
            if(flag[i] == 1 || flag[j] == 1) continue;
            ans += adj[i][j];
        }
    }
    return ans;
}

int main()
{
    srand(time(0));  // 初始化随机数种子

    cin >> n >> m;
    init();
    for (int i = 0; i < m; ++i)
    {
        int u, v;
        cin >> u >> v;
        addEdge(u, v);
    }

    int T = 10; //迭代次数
    T *= n * n;
    int ans = INF;
    while (T--)
    {
        ans = min(Karger(), ans);
        cout << "test ans = " << ans << endl;
    }

    cout << ans << endl;
    system("pause");
    return 0;
}
/*
4 5
1 2 
1 3
1 4
2 3
3 4
*/