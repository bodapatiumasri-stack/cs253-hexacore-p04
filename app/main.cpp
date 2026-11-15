#include <iostream>
#include <vector>

#include "../graph.hpp"
#include "../structA.hpp"
#include "block_cut.h"

using namespace std;

int main() {

    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m;
    cout << "Enter number of vertices and edges: ";
    cin >> n >> m;
    Graph g(n);
    cout << "Enter edges (u v):\n";
    for (int i = 0; i < m; i++) {
        int u, v;
        cin >> u >> v;
        g.addEdge(u, v);
    }
    StructureAResult result =
        analyzeStructureA(g);
    cout << "\n========== CRITICAL VERTICES ==========\n";
    if (result.articulationPoints.empty()) {
        cout << "No articulation points found.\n";
    }
    else {
        cout << "Articulation points: ";
        for (int v : result.articulationPoints) {
            cout << v << " ";
        }
        cout << "\n";
    }
    cout << "\n========== CRITICAL EDGES ==========\n";
    if (result.bridges.empty()) {
        cout << "No bridges found.\n";
    }
    else {
        for (int edgeId : result.bridges) {
            if (edgeId < 0 || edgeId >= g.m) {
                continue;
            }
            int u = g.edges[edgeId].first;
            int v = g.edges[edgeId].second;
            cout << "Edge ID: " << edgeId
                 << " | Edge: (" << u << ", " << v << ")\n";
        }
    }
    cout << "\n========== BICONNECTED COMPONENTS ==========\n";
    cout << "Total components: "
         << result.biconnectedComponents.size() << "\n";
    for (int i = 0;
         i < (int)result.biconnectedComponents.size();
         i++) {
        cout << "Component " << i + 1 << ": ";
        for (int edgeId : result.biconnectedComponents[i]) {
            cout << edgeId << " ";
        }
        cout << "\n";
    }
    BlockCutForest forest =
        buildBlockCutForest(g, result);
    cout << "\n========== BLOCK-CUT FOREST ==========\n";

    cout << "Total forest nodes: "
         << forest.totalNodes << "\n";
    int q;
    cout << "\nEnter number of queries: ";
    cin >> q;
    cout << "\nEnter queries (x y failedVertex):\n";
    for (int i = 0; i < q; i++) {
        int x, y, failedVertex;
        cin >> x >> y >> failedVertex;
        bool disconnected =
            isDisconnectedAfterFailure(
                forest,
                x,
                y,
                failedVertex
            );
        cout << "\nQuery " << i + 1 << ": ";
        if (disconnected) {
            cout << "DISCONNECTED\n";
        }
        else {
            cout << "CONNECTED\n";
        }
    }
    return 0;
}
