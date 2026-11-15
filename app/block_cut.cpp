#include "block_cut.h"

#include <algorithm>
#include <stack>
#include <cmath>

using namespace std;

BlockCutForest buildBlockCutForest(
    const Graph& g,
    const StructureAResult& result
) {
    BlockCutForest forest;
    forest.n = g.n;
    int n = g.n;
    forest.isArticulation.assign(n + 1, false);
    forest.articulationNode.assign(n + 1, -1);
    forest.vertexToNode.assign(n + 1, -1);
    for (int v : result.articulationPoints) {
        if (v >= 1 && v <= n) {
            forest.isArticulation[v] = true;
        }
    }
    vector<vector<int>> blockVertices;
    vector<vector<int>> vertexBlocks(n + 1);
    for (const auto& componentEdges :
         result.biconnectedComponents) {
        vector<int> vertices;
        for (int edgeId : componentEdges) {
            if (edgeId < 0 || edgeId >= g.m) {
                continue;
            }
            int u = g.edges[edgeId].first;
            int v = g.edges[edgeId].second;
            vertices.push_back(u);
            vertices.push_back(v);
        }
        sort(vertices.begin(), vertices.end());
        vertices.erase(
            unique(vertices.begin(), vertices.end()),
            vertices.end()
        );
        if (vertices.empty()) {
            continue;
        }
        int blockId = (int)blockVertices.size();
        blockVertices.push_back(vertices);
        for (int v : vertices) {
            if (v >= 1 && v <= n) {
                vertexBlocks[v].push_back(blockId);
            }
        }
    }
    for (int v = 1; v <= n; v++) {
        if (vertexBlocks[v].empty()) {
            int blockId = (int)blockVertices.size();
            blockVertices.push_back({v});
            vertexBlocks[v].push_back(blockId);
        }
    }
    int blockCount = (int)blockVertices.size();
    int articulationCount = 0;
    for (int v = 1; v <= n; v++) {
        if (forest.isArticulation[v]) {
            forest.articulationNode[v] =
                blockCount + articulationCount;
            articulationCount++;
        }
    }
    forest.totalNodes = blockCount + articulationCount;
    forest.adj.resize(forest.totalNodes);
    for (int v = 1; v <= n; v++) {
        if (forest.isArticulation[v]) {
            forest.vertexToNode[v] =
                forest.articulationNode[v];
        }
        else {
            forest.vertexToNode[v] =
                vertexBlocks[v][0];
        }
    }
    for (int v = 1; v <= n; v++) {
        if (!forest.isArticulation[v]) {
            continue;
        }
        int artNode = forest.articulationNode[v];
        for (int blockId : vertexBlocks[v]) {
            forest.adj[artNode].push_back(blockId);
            forest.adj[blockId].push_back(artNode);
        }
    }
    int N = forest.totalNodes;
    forest.LOG = 1;
    while ((1LL << forest.LOG) <= max(1, N)) {
        forest.LOG++;
    }
    forest.depth.assign(N, 0);
    forest.component.assign(N, -1);
    forest.up.assign(
        forest.LOG,
        vector<int>(N, 0)
    );
    int componentId = 0;
    for (int root = 0; root < N; root++) {
        if (forest.component[root] != -1) {
            continue;
        }
        stack<int> st;
        st.push(root);
        forest.component[root] = componentId;
        forest.up[0][root] = root;
        while (!st.empty()) {
            int u = st.top();
            st.pop();
            for (int v : forest.adj[u]) {
                if (forest.component[v] != -1) {
                    continue;
                }
                forest.component[v] = componentId;
                forest.depth[v] = forest.depth[u] + 1;
                forest.up[0][v] = u;
                st.push(v);
            }
        }
        componentId++;
    }
    for (int j = 1; j < forest.LOG; j++) {
        for (int v = 0; v < N; v++) {
            forest.up[j][v] =
                forest.up[j - 1][
                    forest.up[j - 1][v]
                ];
        }
    }
    return forest;
}
int findLCA(
    const BlockCutForest& forest,
    int u,
    int v
) {
    if (u < 0 || v < 0 ||
        u >= forest.totalNodes ||
        v >= forest.totalNodes) {
        return -1;
    }
    if (forest.component[u] != forest.component[v]) {
        return -1;
    }
    if (forest.depth[u] < forest.depth[v]) {
        swap(u, v);
    }
    int difference = forest.depth[u] - forest.depth[v];
    for (int j = 0; j < forest.LOG; j++) {
        if (difference & (1LL << j)) {
            u = forest.up[j][u];
        }
    }
    if (u == v) {
        return u;
    }
    for (int j = forest.LOG - 1; j >= 0; j--) {
        if (forest.up[j][u] != forest.up[j][v]) {
            u = forest.up[j][u];
            v = forest.up[j][v];
        }
    }
    return forest.up[0][u];
}
int getDistance(
    const BlockCutForest& forest,
    int u,
    int v
) {
    int lca = findLCA(forest, u, v);
    if (lca == -1) {
        return -1;
    }
    return forest.depth[u]
         + forest.depth[v]
         - 2 * forest.depth[lca];
}
bool isDisconnectedAfterFailure(
    const BlockCutForest& forest,
    int x,
    int y,
    int failedVertex
) {
    int n = forest.n;
    if (x < 1 || x > n ||
        y < 1 || y > n ||
        failedVertex < 1 || failedVertex > n) {
        return true;
    }
    if (failedVertex == x || failedVertex == y) {
        return true;
    }
    if (x == y) {
        return false;
    }
    int nodeX = forest.vertexToNode[x];
    int nodeY = forest.vertexToNode[y];
    if (forest.component[nodeX] != forest.component[nodeY]) {
        return true;
    }
    if (!forest.isArticulation[failedVertex]) {
        return false;
    }
    int failedNode =
        forest.articulationNode[failedVertex];
    int totalDistance =
        getDistance(forest, nodeX, nodeY);
    int distanceViaFailed =
        getDistance(forest, nodeX, failedNode)
        + getDistance(forest, failedNode, nodeY);
    return totalDistance == distanceViaFailed;
}

   