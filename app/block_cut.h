#ifndef BLOCK_CUT_H
#define BLOCK_CUT_H

#include <vector>
#include "../graph.hpp"

using namespace std;

struct StructureAResult;
struct BlockCutForest {
    int nodeCount = 0;
    vector<vector<int>> adj;
    vector<int> vertexToNode;
    vector<int> articulationNode;
    vector<int> treeId;
    vector<int> tin;
    vector<int> tout;
    vector<int> depth;
    vector<vector<int>> up;
};

BlockCutForest buildBlockCutForest(
    const Graph& graph,
    const StructureAResult& result
);

bool isDisconnectedAfterFailure(
    const BlockCutForest& forest,
    int x,
    int y,
    int failedVertex
);

#endif

