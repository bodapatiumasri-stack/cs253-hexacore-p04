
#ifndef STRUCTB_HPP
#define STRUCTB_HPP

#include "../../graph.hpp"
#include <vector>

struct StructureBResult {
    std::vector<int> bridges;
    std::vector<std::vector<int>> twoEdgeConnectedComponents;
};

StructureBResult computeStructureB(const Graph& adj,int n);

inline StructureBResult computeStructureB(const Graph& adj) {
    return computeStructureB(adj, adj.n);
}

#endif
