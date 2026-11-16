
#ifndef RANKING_H
#define RANKING_H

#include "block_cut.h"
#include <vector>

using namespace std;

struct CriticalNodeRank {
    int vertex;
    int componentsAfterRemoval;
    int impactScore;
};

vector<CriticalNodeRank> rankCriticalNodes(
    const BlockCutForest& forest
);

void printCriticalNodeRanking(
    const vector<CriticalNodeRank>& ranking
);

#endif