#include "structA.hpp"
#include <algorithm>
#include <stack>
#include <vector>

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

StructureAResult analyzeStructureA(const Graph& g, int n) {
    const auto& adj = g.adj;
    StructureAResult result;
    if (n <= 0) return result;

    std::vector<int> disc(n + 1, 0);
    std::vector<int> low(n + 1, 0);
    std::vector<bool> is_ap(n + 1, false);
    std::stack<StackEdge> edge_stack;
    int timer = 0;

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

            if (frame.neighbor_idx < adj[u].size()) {
                Edge e = adj[u][frame.neighbor_idx++];
                int v = e.to;
                int edge_id = e.id;

                
                if (edge_id == p_edge || v == u) continue;

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
                        result.bridges.push_back(p_edge);
                    }

                    if (low[u] >= disc[parent_node]) {
                        std::vector<int> bcc_edges;

                        while (!edge_stack.empty()) {
                            StackEdge se = edge_stack.top();
                            edge_stack.pop();
                            
                            bcc_edges.push_back(se.id);

                            if (se.id == p_edge) {
                                break;
                            }
                        }

                        std::sort(bcc_edges.begin(), bcc_edges.end());
                        result.biconnectedComponents.push_back(std::move(bcc_edges));
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
    std::sort(result.biconnectedComponents.begin(), result.biconnectedComponents.end());

    return result;
}
