
#include <iostream>
#include <vector>

#include "../graph.hpp"
#include "../structA.hpp"
#include "block_cut.h"

using namespace std;

int main() {
    Graph graph;
    cout << "Enter number of vertices and edges: ";
    cin >> graph.n >> graph.m;
    if (!cin || graph.n < 0 || graph.m < 0) {
        cout << "Invalid graph input.\n";
        return 1;
    }
    graph.adj.resize(graph.n + 1);
    graph.edges.resize(graph.m);
    cout << "Enter the edges (u v):\n";
    for (int id = 0; id < graph.m; id++) {
        int u, v;
        cin >> u >> v;
        if (!cin || u < 1 || u > graph.n ||
            v < 1 || v > graph.n) {
            cout << "Invalid edge input.\n";
            return 1;
        }
        graph.edges[id] = {u, v};
        graph.adj[u].push_back({v, id});
        if (u != v) {
            graph.adj[v].push_back({u, id});
        }
    }
    StructureAResult result =
        analyzeStructureA(graph);
    BlockCutForest forest =
        buildBlockCutForest(graph, result);
    cout << "\n========== GRAPH ANALYSIS ==========\n";
    cout << "Articulation Points: ";
    if (result.articulationPoints.empty()) {
        cout << "None";
    } else {
        for (int v : result.articulationPoints) {
            cout << v << " ";
        }
    }
    cout << "\n";
    cout << "Bridges (edge IDs): ";
    if (result.bridges.empty()) {
        cout << "None";
    } else {
        for (int id : result.bridges) {
            cout << id << " ";
        }
    }
    cout << "\n";
    cout << "Number of Biconnected Components: "
         << result.biconnectedComponents.size() << "\n";
    cout << "Number of Block-Cut Forest Nodes: "
         << forest.nodeCount << "\n";
    int q;
    cout << "\nEnter number of queries: ";
    cin >> q;
    if (!cin || q < 0) {
        cout << "Invalid query count.\n";
        return 1;
    }
    cout << "Enter each query as: x y failedVertex\n";
    cout << "\n========== QUERY RESULTS ==========\n";
    for (int i = 1; i <= q; i++) {
        int x, y, failedVertex;
        cin >> x >> y >> failedVertex;
        if (!cin) {
            cout << "Invalid query input.\n";
            return 1;
        }
        bool disconnected =
            isDisconnectedAfterFailure(
                forest,
                x,
                y,
                failedVertex
            );
        cout << "Query " << i << ": ";
        if (disconnected) {
            cout << "DISCONNECTED\n";
        } else {
            cout << "CONNECTED\n";
        }
    }
    cout << "\n========== ANALYSIS COMPLETE ==========\n";
    return 0;
}