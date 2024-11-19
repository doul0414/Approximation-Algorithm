#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>
#include <cstdlib>  
#include <ctime>   
#include <queue>
#include <cstring> 
using namespace std;

const int INF = 0x7fffffff;
const int MAXN = 1005;
const int NMAX = 1005;

struct Graph {
    int adj[MAXN][MAXN];    // 邻接矩阵
    int W[MAXN];            // 每个顶点的权重和
    int flag[MAXN];         // 标记是否被合并
    int n;                  // 矩阵顶点数
    int m;                  // 边数
    int r;                  //合并后真正的顶点数

    void addEdge(int u, int v) {
        adj[u][v]++;
        adj[v][u]++;
    }

    void InitGraph() {
        r = n;
        for(int i = 1; i <= n; ++i) {
            flag[i] = 0;
            W[i] = 0;
            for(int j = 1; j <= n; ++j) {
                if (adj[i][j] > 0 && j >= i) {
                    W[i] += adj[i][j];
                }
            }
        }
    }

    void copyGraph(const Graph &original) {
        n = original.n;
        m = original.m;
        r = original.r;
        for(int i = 1; i <= n; ++i) {
            flag[i] = original.flag[i];
            W[i] = original.W[i];
            for(int j = 1; j <= n; ++j) {
                adj[i][j] = original.adj[i][j];
            }
        }
    }

};

/*copy的dinic算法************************************************************************/
/*copy的dinic算法************************************************************************/
struct Edge {
    int next, to, c;
} edge[NMAX]; // 边数组

int head[MAXN], tot = 1;
int cur[MAXN], dep[MAXN];
int S, T; // 源点和汇点

void addedge(int u, int v, int c) {
    edge[++tot] = {head[u], v, c};
    head[u] = tot;
    edge[++tot] = {head[v], u, 0}; // 添加反向边，初始容量为 0
    head[v] = tot;
}

bool bfs_dinic() {
    memset(dep, 0, sizeof(dep));
    queue<int> q;
    q.push(S);
    dep[S] = 1;
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (int i = head[u]; i; i = edge[i].next) {
            int v = edge[i].to;
            if (dep[v] == 0 && edge[i].c > 0) {
                dep[v] = dep[u] + 1;
                q.push(v);
                if (v == T) return true;
            }
        }
    }
    return false;
}

int dfs_dinic(int u, int inflow) {
    if (u == T) return inflow;
    int outflow = 0;
    for (int &i = cur[u]; i; i = edge[i].next) {
        int v = edge[i].to;
        if (dep[v] == dep[u] + 1 && edge[i].c > 0) {
            int flow = dfs_dinic(v, min(inflow, edge[i].c));
            if (flow > 0) {
                edge[i].c -= flow;
                edge[i ^ 1].c += flow;
                outflow += flow;
                inflow -= flow;
                if (inflow == 0) break;
            }
        }
    }
    if (outflow == 0) dep[u] = 0;
    return outflow;
}

int dinic_max_flow() {
    int sum = 0;
    while (bfs_dinic()) {
        memcpy(cur, head, sizeof(head));
        sum += dfs_dinic(S, INT_MAX);
    }
    return sum;
}

// 计算全局最小割
int dinic_algorithm(Graph &G) {
    int min_cut = INT_MAX;

    // 获取未被合并的节点列表
    vector<int> nodes;
    for (int i = 1; i <= G.n; ++i) {
        if (G.flag[i] == 0) {
            nodes.push_back(i);
        }
    }

    // 遍历所有节点对
    for (size_t i = 0; i < nodes.size(); ++i) {
        for (size_t j = i + 1; j < nodes.size(); ++j) {
            // 重置网络
            memset(head, 0, sizeof(head));
            tot = 1;

            // 建立残余网络
            for (int u = 1; u <= G.n; ++u) {
                if (G.flag[u] == 1) continue;
                for (int v = 1; v <= G.n; ++v) {
                    if (G.flag[v] == 1) continue;
                    if (G.adj[u][v] > 0) {
                        addedge(u, v, G.adj[u][v]);
                    }
                }
            }

            // 设置源点和汇点
            S = nodes[i];
            T = nodes[j];

            // 计算最大流
            int flow = dinic_max_flow();
            min_cut = min(min_cut, flow);

            // 如果最小割为 0，直接返回
            if (min_cut == 0) return 0;
        }
    }

    return min_cut;
}
/*************************************************************************/
/*************************************************************************/

void contract(Graph &G, int u, int v) //时间复杂度为O(n)
{
    // 合并节点v到节点u
    for(int i = 1; i <= G.n; ++i)
    {
        G.adj[u][i] += G.adj[v][i];
        G.adj[i][u] += G.adj[i][v];
    }
    G.adj[u][u] = 0;  // 自环边为0
    G.flag[v] = 1;  // 标记v已经被合并
    G.r--;  // 合并后真正的顶点数减1

    //O(n)时间内更新W数组
    for(int i = 1; i <= G.n; ++i)
    {
        if(i < u && v < i && G.flag[i] == 0)  // 只有i在u和v之间的行才需要更新W[i]
        {
            G.W[i] += G.adj[i][v];
        }
        else if(i > u && v > i && G.flag[i] == 0)  
        {
            G.W[i] -= G.adj[i][v];
        }
    }
    G.W[u] = 0;  // 单独计算W[u]
    for(int i = u + 1; i <= G.n; ++i)
    {
        if(G.flag[i] == 1) continue;
        G.W[u] += G.adj[u][i]; 
    }

}

pair<int, int> RandomPick(Graph &G)
{
    int sum = 0;
    int first = INF;
    for(int i = 1; i <= G.n; ++i)
    {
        if(G.flag[i] == 1) continue;;
        sum += G.W[i];
        if (first == INF) first = i;
    }
    
    int target = rand() % sum + 1; // 随机选一条边
    int i = first, w = G.W[first];
    while (target > w)  // 找到目标区间对应的i
    {
        ++i;
        if(G.flag[i] == 1) continue;
        w += G.W[i];
    }
  
    target = target - (w - G.W[i]);
    int j = i;
    while (target > 0)
    {
        ++j;
        if (G.flag[j] == 1) continue;
        target -= G.adj[i][j];
    }
    cout <<"test:"<<i<<" "<<j<<endl;
    return {i, j};
}

int contract2(Graph &G)
{
    if(G.r <= 6) return dinic_algorithm(G);
    int t = ceil(G.r *(1 - 1 / sqrt(2.0))) - 1; 

    Graph* G1 = new Graph();
    Graph* G2 = new Graph();
    G1->copyGraph(G);
    G2->copyGraph(G);

    while(t--)
    {
        pair<int, int> e1 = RandomPick(*G1);
        pair<int, int> e2 = RandomPick(*G2);
        contract(*G1, e1.first, e1.second);
        contract(*G2, e2.first, e2.second);
    } 
    int cut1 = contract2(*G1);
    int cut2 = contract2(*G2);
    return min(cut1, cut2);
}

int karger_Stein(Graph &G, int t)   //t为迭代次数
{
    int min_cut = INF;
    int alpha = ceil(log(G.n)); //迭代因子 log（n）
    
    for(int i = 1; i <= t * alpha; ++i)
    {
        int cut = contract2(G);
        min_cut = min(min_cut, cut);
    }
    return min_cut;
}

int main()
{
   srand(time(0));  // 初始化随机数种子

    Graph* G = new Graph();
    cin >> G->n >> G->m;
  
    for(int i = 0; i < G->m; ++i)
    {
        int u, v;
        cin >> u >> v;
        G->addEdge(u, v);
    }

    G->InitGraph();

    cout<<karger_Stein(*G, 10)<<endl; //这里设t=10，这样成功率＞0.9999

    delete G;
    system("pause");
    return 0;
}

/*
输入示例：
4 5
1 2 
1 3
1 4
2 3
3 4
样例输出:
2

示例2
输入示例：
16 59
1 2
1 3
1 4
1 5
1 6
1 7
1 8
2 3
2 4
2 5
2 6
2 7
2 8
3 4
3 5
3 6
3 7
3 8
4 5
4 6
4 7
4 8
5 6
5 7
5 8
6 7
6 8
7 8
9 10
9 11
9 12
9 13
9 14
9 15
9 16
10 11
10 12
10 13
10 14
10 15
10 16
11 12
11 13
11 14
11 15
11 16
12 13
12 14
12 15
12 16
13 14
13 15
13 16
14 15
14 16
15 16
1 9
2 10
3 11
样例输出：
3
*/