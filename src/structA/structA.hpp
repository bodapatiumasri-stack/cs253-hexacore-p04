#ifndef STRUCT_A_HPP
#define STRUCT_A_HPP

#include "../../graph.hpp"
#include <vector>

struct StructureAResult {
    std::vector<int> articulationPoints;
    std::vector<int> bridges;
    std::vector<std::vector<int>> biconnectedComponents;
};

StructureAResult analyzeStructureA(const Graph& g, int n);

inline StructureAResult analyzeStructureA(const Graph& g) {
    return analyzeStructureA(g, g.n);
}

#endif
