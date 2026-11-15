#ifndef STRUCT_A_HPP
#define STRUCT_A_HPP

#include "../common/graph.hpp"
#include <vector>

struct StructureAResult {
    std::vector<int> articulationPoints;
    std::vector<int> bridges;
    std::vector<std::vector<int>> biconnectedComponents;
};

StructureAResult analyzeStructureA(const Graph& adj, int n);

#endif
