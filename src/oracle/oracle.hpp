#ifndef ORACLE_HPP
#define ORACLE_HPP
#include <string>
#include <vector>
#include "graph.hpp"

int countComponents(const GraphData& g,
                    int removedVertex = -1,
                    int removedEdge = -1);

std::vector<int> oracleArticulationPoints(const GraphData& g);

std::vector<int> oracleBridges(const GraphData& g);

bool oracleTooLarge(const GraphData& g);

bool compareResults(const std::string& label,
                    std::vector<int> oracle,
                    std::vector<int> other);

bool runOracleReport(const GraphData& g,
                     const std::vector<int>& structureA_ArticulationPoints,
                     const std::vector<int>& structureA_Bridges,
                     const std::vector<int>& structureB_Bridges);

#endif