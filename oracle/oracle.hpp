#ifndef ORACLE_HPP
#define ORACLE_HPP

#include <string>
#include <vector>

#include "../graph.hpp"   

using AdjList = std::vector<std::vector<Edge>>;

struct OracleResult {
    int originalComponentCount = 0;
    std::vector<int> componentsAfterVertexRemoval;  
    std::vector<int> componentsAfterEdgeRemoval;    
    std::vector<int> articulationPoints;            
    std::vector<int> bridges;                      
};

int countComponents(const AdjList& adj, int n,
                    int removedVertex = -1, int removedEdge = -1);

OracleResult runOracle(const Graph& graph);

bool oracleTooLarge(const Graph& graph);

bool compareResults(const std::string& label,
                    std::vector<int> oracle, std::vector<int> other);

bool runOracleReport(const Graph& graph,
                     const std::vector<int>& structureA_ArticulationPoints,
                     const std::vector<int>& structureA_Bridges,
                     const std::vector<int>& structureB_Bridges);

#endif
