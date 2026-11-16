#include "ranking.h"

#include <algorithm>
#include <iostream>

using namespace std;
vector<CriticalNodeRank> rankCriticalNodes(
    const BlockCutForest& forest
) {
    vector<CriticalNodeRank> ranking;
    int originalComponents = 0;
    for (int id : forest.component) {
        originalComponents = max(
            originalComponents,
            id + 1
        );
    }
    for (int v = 1; v <= forest.n; v++) {
        if (!forest.isArticulation[v]) {
            continue;
        }
        int node = forest.articulationNode[v];
        if (node < 0 || node >= forest.totalNodes) {
            continue;
        }
        int branches = forest.adj[node].size();
        
        int afterRemoval =
            originalComponents - 1 + branches;
        int score =
            afterRemoval - originalComponents;
        CriticalNodeRank entry;
        entry.vertex = v;
        entry.componentsAfterRemoval = afterRemoval;
        entry.impactScore = score;
        ranking.push_back(entry);
    }
    sort(
        ranking.begin(),
        ranking.end(),
        [](const CriticalNodeRank& a,
           const CriticalNodeRank& b) {

            if (a.impactScore != b.impactScore) {
                return a.impactScore > b.impactScore;
            }

            return a.vertex < b.vertex;
        }
    );

    return ranking;
}

void printCriticalNodeRanking(
    const vector<CriticalNodeRank>& ranking
) {
    cout << "\n========== CRITICAL NODE RANKING ==========\n";

    if (ranking.empty()) {
        cout << "No critical nodes found.\n";
        return;
    }

    cout << "Rank\tVertex\tComponents After Removal\tScore\n";

    for (int i = 0; i < (int)ranking.size(); i++) {

        cout << i + 1 << "\t"
             << ranking[i].vertex << "\t"
             << ranking[i].componentsAfterRemoval << "\t\t\t"
             << ranking[i].impactScore << "\n";
    }

    cout << "===========================================\n";
}
