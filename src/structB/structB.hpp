
#ifndef STRUCTB_HPP
#define STRUCTB_HPP

#include "../common/graph.hpp"
#include <vector>

struct StructureBResult {
    std::vector<int> bridges;
    std::vector<std::vector<int>> twoEdgeConnectedComponents;
};

StructureBResult computeStructureB(const GraphData& g);

#endif
