#ifndef STRUCTA_HPP
#define STRUCTA_HPP

#include "../common/graph.hpp"
#include <vector>

struct BiconnectedComponent {
    std::vector<int> vertices;  
    std::vector<int> edges;     
};

struct StructureAResult {
    std::vector<int> articulationPoints;
    std::vector<int> bridges;
    std::vector<BiconnectedComponent> biconnectedComponents;
};

StructureAResult computeStructureA(const GraphData& g);

#endif
