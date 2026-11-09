#include "structA.hpp"
#include <vector>
#include <stack>
#include <algorithm>
#include <set>

namespace {

struct StackEdge {
    int u;
    int v;
    int id;
};

struct DFSFrame {
    int u;
    int parent_edge_id;
    size_t neighbor_idx;
};

}

StructureAResult computeStructureA(const GraphData& g) {
    StructureAResult result;
    int n = g.n;

    if (n <= 0) return result;

    std::vector<int> disc(n + 1, 0);
    std::vector<int> low(n + 1, 0);
    std::vector<bool> is_ap(n + 1, false);
    
    int timer = 0;
    std::stack<StackEdge> edge_stack;

    for (int start_node = 1; start_node <= n; ++start_node) {
        if (disc[start_node] != 0) continue;

        std::vector<DFSFrame> call_stack;
        
        timer++;
        disc[start_node] = low[start_node] = timer;
        call_stack.push_back({start_node, -1, 0});

        int root_children = 0;

        while (!call_stack.empty()) {
            DFSFrame& frame = call_stack.back();
            int u = frame.u;
            int p_edge = frame.parent_edge_id;

            if (frame.neighbor_idx < g.adj[u].size()) {
                Edge e = g.adj[u][frame.neighbor_idx++];
                int v = e.to;
                int edge_id = e.id;

                if (edge_id == p_edge) continue;

                if (disc[v] != 0) {
                    low[u] = std::min(low[u], disc[v]);
                    if (disc[v] < disc[u]) {
                        edge_stack.push({u, v, edge_id});
                    }
                } else {
                    edge_stack.push({u, v, edge_id});
                    
                    if (p_edge == -1) {
                        root_children++;
                    }

                    timer++;
                    disc[v] = low[v] = timer;
                    call_stack.push_back({v, edge_id, 0});
                }
            } else {
                call_stack.pop_back();

                if (!call_stack.empty()) {
                    int parent_node = call_stack.back().u;
                    int parent_p_edge = call_stack.back().parent_edge_id;

                    low[parent_node] = std::min(low[parent_node], low[u]);

                    if (parent_p_edge != -1 && low[u] >= disc[parent_node]) {
                        is_ap[parent_node] = true;
                    }

                    if (low[u] > disc[parent_node]) {
                        for (const auto& edge_info : g.adj[parent_node]) {
                            if (edge_info.to == u) {
                                result.bridges.push_back(edge_info.id);
                                break;
                            }
                        }
                    }

                    if (low[u] >= disc[parent_node]) {
                        BiconnectedComponent bcc;
                        std::set<int> bcc_vertices;

                        while (!edge_stack.empty()) {
                            StackEdge se = edge_stack.top();
                            edge_stack.pop();
                            
                            bcc.edges.push_back(se.id);
                            bcc_vertices.insert(se.u);
                            bcc_vertices.insert(se.v);

                            if ((se.u == parent_node && se.v == u) || (se.u == u && se.v == parent_node)) {
                                break;
                            }
                        }

                        bcc.vertices.assign(bcc_vertices.begin(), bcc_vertices.end());
                        std::sort(bcc.edges.begin(), bcc.edges.end());
                        result.biconnectedComponents.push_back(bcc);
                    }
                }
            }
        }

        is_ap[start_node] = (root_children >= 2);
    }

    for (int i = 1; i <= n; ++i) {
        if (is_ap[i]) {
            result.articulationPoints.push_back(i);
        }
    }

    std::sort(result.articulationPoints.begin(), result.articulationPoints.end());
    std::sort(result.bridges.begin(), result.bridges.end());

    std::sort(result.biconnectedComponents.begin(), result.biconnectedComponents.end(),
        [](const BiconnectedComponent& a, const BiconnectedComponent& b) {
            return a.vertices < b.vertices;
        }
    );

    return result;
}
