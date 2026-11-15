#include "block_cut.h"
#include "../structA.hpp"

#include <algorithm>
#include <stack>
#include <utility>

using namespace std;

BlockCutForest buildBlockCutForest(
    const Graph& graph,
    const StructureAResult& result
) {
    BlockCutForest forest;
    int n = graph.n;
    forest.vertexToNode.assign(n + 1, -1);
    forest.articulationNode.assign(n + 1, -1);
    vector<bool> isArticulation(n + 1, false);
    for (int v : result.articulationPoints) {
        if (v >= 1 && v <= n) {
            isArticulation[v] = true;
        }
    }
    vector<vector<int>> blocks;
    for (const auto& component : result.biconnectedComponents) {
        vector<int> vertices;
        for (int edgeId : component) {
            if (edgeId < 0 ||
                edgeId >= (int)graph.edges.size()) {
                continue;
            }
            int u = graph.edges[edgeId].first;
            int v = graph.edges[edgeId].second;
            if (u >= 1 && u <= n) {
                vertices.push_back(u);
            }
            if (v >= 1 && v <= n) {
                vertices.push_back(v);
            }
        }
        sort(vertices.begin(), vertices.end());
        vertices.erase(
            unique(vertices.begin(), vertices.end()),
            vertices.end()
        );
        if (!vertices.empty()) {
            blocks.push_back(vertices);
        }
    }
    int blockCount = (int)blocks.size();
    vector<int> blockNode(blockCount);
    for (int i = 0; i < blockCount; i++) {
        blockNode[i] = forest.nodeCount;
        forest.nodeCount++;
    }
    for (int v = 1; v <= n; v++) {
        if (isArticulation[v]) {
            forest.articulationNode[v] = forest.nodeCount;
            forest.nodeCount++;
        }
    }
    forest.adj.resize(forest.nodeCount);
    for (int i = 0; i < blockCount; i++) {
        int bNode = blockNode[i];
        for (int v : blocks[i]) {
            if (isArticulation[v]) {
                int aNode = forest.articulationNode[v];

                forest.adj[bNode].push_back(aNode);
                forest.adj[aNode].push_back(bNode);
            } else {
                forest.vertexToNode[v] = bNode;
            }
        }
    }
    for (int v = 1; v <= n; v++) {
        if (isArticulation[v]) {
            forest.vertexToNode[v] =
                forest.articulationNode[v];
        }
    }
    for (int v = 1; v <= n; v++) {
        if (forest.vertexToNode[v] == -1) {
            forest.vertexToNode[v] = forest.nodeCount;
            forest.nodeCount++;
            forest.adj.push_back({});
        }
    }
    int N = forest.nodeCount;
    forest.treeId.assign(N, -1);
    forest.tin.assign(N, 0);
    forest.tout.assign(N, 0);
    forest.depth.assign(N, 0);
    vector<int> parent(N, -1);
    int timer = 0;
    int treeNumber = 0;
    for (int root = 0; root < N; root++) {
        if (forest.treeId[root] != -1) {
            continue;
        }
        stack<pair<int, bool>> st;
        forest.treeId[root] = treeNumber;
        parent[root] = root;
        st.push({root, false});
        while (!st.empty()) {
            auto [u, exiting] = st.top();
            st.pop();
            if (exiting) {
                forest.tout[u] = timer;
                continue;
            }
            forest.tin[u] = ++timer;
            st.push({u, true});
            for (int i = (int)forest.adj[u].size() - 1;
                 i >= 0; i--) {
                int v = forest.adj[u][i];
                if (v == parent[u]) {
                    continue;
                }
                if (forest.treeId[v] != -1) {
                    continue;
                }
                forest.treeId[v] = treeNumber;
                parent[v] = u;
                forest.depth[v] = forest.depth[u] + 1;
                st.push({v, false});
            }
        }
        treeNumber++;
    }
    int LOG = 1;
    while ((1LL << LOG) <= max(1, N)) {
        LOG++;
    }
    forest.up.assign(LOG, vector<int>(N));
    for (int v = 0; v < N; v++) {
        forest.up[0][v] = parent[v];
    }
    for (int j = 1; j < LOG; j++) {
        for (int v = 0; v < N; v++) {
            forest.up[j][v] =
                forest.up[j - 1][forest.up[j - 1][v]];
        }
    }
    return forest;
}
static bool isAncestor(
    const BlockCutForest& forest,
    int u,
    int v
) {
    return forest.tin[u] <= forest.tin[v] &&
           forest.tout[v] <= forest.tout[u];
}
static int findLCA(
    const BlockCutForest& forest,
    int u,
    int v
) {
    if (isAncestor(forest, u, v)) {
        return u;
    }
    if (isAncestor(forest, v, u)) {
        return v;
    }
    int LOG = (int)forest.up.size();
    for (int j = LOG - 1; j >= 0; j--) {
        int ancestor = forest.up[j][u];

        if (!isAncestor(forest, ancestor, v)) {
            u = ancestor;
        }
    }
    return forest.up[0][u];
}
static int getDistance(
    const BlockCutForest& forest,
    int u,
    int v
) {
    int lca = findLCA(forest, u, v);
    return forest.depth[u] +
           forest.depth[v] -
           2 * forest.depth[lca];
}
bool isDisconnectedAfterFailure(
    const BlockCutForest& forest,
    int x,
    int y,
    int failedVertex
) {
    int n = (int)forest.vertexToNode.size() - 1;
    if (x < 1 || x > n ||
        y < 1 || y > n ||
        failedVertex < 1 || failedVertex > n) {
        return false;
    }
    if (failedVertex == x || failedVertex == y) {
        return true;
    }
    if (x == y) {
        return false;
    }
    int nodeX = forest.vertexToNode[x];
    int nodeY = forest.vertexToNode[y];
    if (forest.treeId[nodeX] != forest.treeId[nodeY]) {
        return true;
    }
    int failedNode =
        forest.articulationNode[failedVertex];
    if (failedNode == -1) {
        return false;
    }
    if (forest.treeId[failedNode] != forest.treeId[nodeX]) {
    return false;
}
    int totalDistance =
        getDistance(forest, nodeX, nodeY);
    int distanceThroughFailed =
        getDistance(forest, nodeX, failedNode) +
        getDistance(forest, failedNode, nodeY);
    return totalDistance == distanceThroughFailed;
}
