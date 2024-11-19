#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <unordered_set>
#include <climits>

using namespace std;

const int MAXN = 1000; // 最大节点数

struct Edge {
    int to;
    double weight;
};

vector<Edge> adj[MAXN]; // 图的邻接表表示
int n; // 节点数

// 添加边
void addEdge(int u, int v, double w) {
    adj[u].push_back({v, w});
    adj[v].push_back({u, w}); // 无向图，双向添加
}

// 路径的定义
struct Path {
    int vertice;    // 路径的终点
    double weight;  // 路径总权重

    Path() : vertice(-1), weight(0) {}
};

// FindPaths函数的声明
vector<Path> FindPaths(const vector<Path> &Pl, const vector<int> &V_set, int h);

// 获取最大权重的k-路径的函数
double GetKPath(int k, int t) {
    double maxk = -1;

    // 初始路径集合Pl，包含空路径
    vector<Path> Pl;
    Pl.push_back(Path());

    // 节点集合V_set，包含所有节点
    vector<int> V_set;
    for (int i = 0; i < n; ++i) {
        V_set.push_back(i);
    }

    while (t--) {
        vector<Path> res = FindPaths(Pl, V_set, k);
        for (const auto &p : res) {
            // 在这里，可以根据需求选择目标节点，例如节点n - 1
            // 也可以选择所有长度为k的路径中的最大权重
            if (p.weight > maxk) {
                maxk = p.weight;
            }
        }
    }
    return maxk;
}

vector<Path> FindPaths(const vector<Path> &Pl, const vector<int> &V_set, int h) {
    vector<Path> Pl_h;

    if (h == 1) {
        if (Pl.size() == 1 && Pl[0].vertice == -1) {
            // 情况 2.1：Pl = {ρ∅}
            for (int u : V_set) {
                Path p;
                p.vertice = u;
                p.weight = 0; // 初始路径权重为 0
                Pl_h.push_back(p);
            }
        } else {
            // 情况 2.2
            for (const Path &rho_l : Pl) {
                int w = rho_l.vertice;
                for (int u : V_set) {
                    // 检查是否存在边 (w, u)
                    bool found = false;
                    double edge_weight = 0;
                    for (const auto &edge : adj[w]) {
                        if (edge.to == u) {
                            found = true;
                            edge_weight = edge.weight;
                            break;
                        }
                    }
                    if (found) {
                        // 创建新路径
                        Path new_path;
                        new_path.vertice = u;
                        new_path.weight = rho_l.weight + edge_weight;

                        // 更新 Pl_h 中的路径
                        bool updated = false;
                        for (auto &p : Pl_h) {
                            if (p.vertice == u) {
                                if (p.weight < new_path.weight) {
                                    p.weight = new_path.weight;
                                }
                                updated = true;
                                break;
                            }
                        }
                        if (!updated) {
                            Pl_h.push_back(new_path);
                        }
                    }
                }
            }
        }
        return Pl_h;
    } else {
        int loop_times = static_cast<int>(ceil(3 * pow(2, h)));
        for (int loop = 0; loop < loop_times; ++loop) {
            // 随机划分 V_set 为 VL 和 VR
            vector<int> VL, VR;
            for (int v : V_set) {
                if (rand() % 2 == 0)
                    VL.push_back(v);
                else
                    VR.push_back(v);
            }

            int h1 = static_cast<int>(ceil(h / 2.0));
            int h2 = h - h1;

            // 更新 V_set_L
            unordered_set<int> Pl_vertices;
            for (const auto &path : Pl) {
                Pl_vertices.insert(path.vertice);
            }
            vector<int> V_set_L = VL;
            V_set_L.erase(remove_if(V_set_L.begin(), V_set_L.end(),
                                    [&](int v) { return Pl_vertices.count(v) > 0; }),
                          V_set_L.end());

            vector<Path> Pl_L = FindPaths(Pl, V_set_L, h1);
            if (!Pl_L.empty()) {
                // 更新 V_set_R
                unordered_set<int> Pl_L_vertices;
                for (const auto &path : Pl_L) {
                    Pl_L_vertices.insert(path.vertice);
                }
                vector<int> V_set_R = VR;
                V_set_R.erase(remove_if(V_set_R.begin(), V_set_R.end(),
                                        [&](int v) { return Pl_L_vertices.count(v) > 0; }),
                              V_set_R.end());

                vector<Path> Pl_R = FindPaths(Pl_L, V_set_R, h2);

                // 将 Pl_R 中的路径更新到 Pl_h
                for (const Path &path : Pl_R) {
                    // 更新 Pl_h 中的路径
                    bool updated = false;
                    for (auto &p : Pl_h) {
                        if (p.vertice == path.vertice) {
                            if (p.weight < path.weight) {
                                p.weight = path.weight;
                            }
                            updated = true;
                            break;
                        }
                    }
                    if (!updated) {
                        Pl_h.push_back(path);
                    }
                }
            }
        }
        return Pl_h;
    }
}

// 主函数
int main() {
    srand(static_cast<unsigned int>(time(nullptr))); // 初始化随机数种子

    // 输入节点数n和边数m
    int m;
    cout << "n m : " << endl;
    cin >> n >> m;

    // 输入边的信息
    cout << "u v w :" << endl;
    for (int i = 0; i < m; ++i) {
        int u, v;
        double w;
        cin >> u >> v >> w;
        addEdge(u, v, w);
    }

    int k;
    cout << "k:" << endl;
    cin >> k;

    int t;
    cout << "t:" << endl;
    cin >> t; // 迭代次数

    double max_weight = GetKPath(k+1, t);

    if (max_weight > -1) {
        cout << "Maxmum " << k << "-Path 's Weight:" << max_weight << endl;
    } else {
        cout << "Not Find" << k << " -Path." << endl;
    }
    system("pause");
    return 0;
}

// 样例输入:
/*
6 7
0 1 3.0
0 2 2.0
1 3 4.0
2 3 1.0
3 4 5.0
4 5 2.0
1 5 6.0
4
10

*/