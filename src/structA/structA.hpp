#pragma once
#include "../common/graph.hpp"
#include <vector>

struct StructureAResult {
    std::vector<int> articulationPoints;
    std::vector<int> bridges;
    std::vector<std::vector<int>> biconnectedComponents;
};

StructureAResult analyzeStructureA(
    const std::vector<std::vector<Edge>>& adj,
    int n
);

inline StructureAResult analyzeStructureA(const GraphData& g) {
    return analyzeStructureA(g.adj, g.n);
}
