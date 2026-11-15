
#ifndef BLOCK_CUT_H
#define BLOCK_CUT_H

#include "../graph.hpp"
#include "../structA.hpp"

#include <vector>

using namespace std;

struct BlockCutForest {
    int n = 0;
    int totalNodes = 0;
    int LOG = 0;
    vector<vector<int>> adj;
    vector<int> vertexToNode;
    vector<bool> isArticulation;
    vector<int> articulationNode;
    vector<int> component;
    vector<int> depth;
    vector<vector<int>> up;
};
BlockCutForest buildBlockCutForest(
    const Graph& g,
    const StructureAResult& result
);
int findLCA(
    const BlockCutForest& forest,
    int u,
    int v
);
int getDistance(
    const BlockCutForest& forest,
    int u,
    int v
);
bool isDisconnectedAfterFailure(
    const BlockCutForest& forest,
    int x,
    int y,
    int failedVertex
);

#endif




