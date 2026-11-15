#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include "../src/structA/structA.hpp"
#include "../src/structB/structB.hpp"
#include "oracle.hpp"
using namespace std;
static int failures = 0;
static GraphData makeGraph(int n, const vector<pair<int, int>>& edgeList) {
    GraphData g;
    g.n = n;
    g.m = (int)edgeList.size();
    g.edges = edgeList;
    g.adj.assign(n + 1, {});
    for (int id = 0; id < g.m; id++) {
        int u = edgeList[id].first;
        int v = edgeList[id].second;
        g.adj[u].push_back({v, id});
        g.adj[v].push_back({u, id});
    }
    return g;
}

static vector<int> sorted(vector<int> v) {
    sort(v.begin(), v.end());
    v.erase(unique(v.begin(), v.end()), v.end());
    return v;
}

static void runNamed(const string& name, const GraphData& g) {
    cout << "### " << name << "\n";
    StructureAResult a = computeStructureA(g);
    StructureBResult b = computeStructureB(g);
    bool ok = runOracleReport(g, a.articulationPoints, a.bridges, b.bridges);
    if (!ok) failures++;
}

static bool quietCheck(const GraphData& g, const string& label) {
    StructureAResult a = computeStructureA(g);
    StructureBResult b = computeStructureB(g);

    vector<int> ap = oracleArticulationPoints(g);
    vector<int> br = oracleBridges(g);

    bool ok = sorted(a.articulationPoints) == ap &&
              sorted(a.bridges) == br &&
              sorted(b.bridges) == br;

    if (!ok) {
        cout << "!!! MISMATCH on " << label << " (n=" << g.n << ", m=" << g.m << ")\n";
        cout << "Edges (id: u v):";
        for (int i = 0; i < g.m; i++) {
            cout << " " << i << ":" << g.edges[i].first << "-" << g.edges[i].second;
        }
        cout << "\n";
        runOracleReport(g, a.articulationPoints, a.bridges, b.bridges);
    }
    return ok;
}

static unsigned long long rngState = 88172645463325252ULL;
static unsigned int nextRand() {
    rngState ^= rngState << 13;
    rngState ^= rngState >> 7;
    rngState ^= rngState << 17;
    return (unsigned int)(rngState >> 11);
}

static GraphData randomGraph(int n, int m) {
    set<pair<int, int>> used;
    vector<pair<int, int>> edgeList;
    int maxEdges = n * (n - 1) / 2;
    if (m > maxEdges) m = maxEdges;
    while ((int)edgeList.size() < m) {
        int u = (int)(nextRand() % n) + 1;
        int v = (int)(nextRand() % n) + 1;
        if (u == v) continue;
        pair<int, int> key = {min(u, v), max(u, v)};
        if (used.count(key)) continue;
        used.insert(key);
        edgeList.push_back({u, v});
    }
    return makeGraph(n, edgeList);
}

int main() {
   
    runNamed("Path 1-2-3-4-5", makeGraph(5, {{1, 2}, {2, 3}, {3, 4}, {4, 5}}));
    runNamed("Cycle of 4", makeGraph(4, {{1, 2}, {2, 3}, {3, 4}, {4, 1}}));
    runNamed("Star", makeGraph(5, {{1, 2}, {1, 3}, {1, 4}, {1, 5}}));
    runNamed("Disconnected + isolated vertex", makeGraph(6, {{1, 2}, {2, 3}, {4, 5}}));
    runNamed("Single vertex", makeGraph(1, {}));
    runNamed("Triangle with tail",
             makeGraph(4, {{1, 2}, {2, 3}, {3, 1}, {3, 4}}));
    runNamed("Two triangles joined by a path",
             makeGraph(7, {{1, 2}, {2, 3}, {3, 1}, {3, 4}, {4, 5}, {5, 6}, {6, 7}, {7, 5}}));

   
    const int TRIALS = 500;
    int bad = 0;
    for (int t = 0; t < TRIALS; t++) {
        int n = (int)(nextRand() % 25) + 1;                
        int maxEdges = n * (n - 1) / 2;
        int m = maxEdges == 0 ? 0 : (int)(nextRand() % (min(maxEdges, 40) + 1));
        GraphData g = randomGraph(n, m);
        if (!quietCheck(g, "random trial " + to_string(t))) bad++;
    }
    cout << "Random stress test: " << (TRIALS - bad) << "/" << TRIALS << " graphs matched.\n";
    failures += bad;

    cout << "\n" << (failures == 0 ? "ALL CHECKS MATCHED" : "SOME CHECKS FAILED")
         << " (failures: " << failures << ")\n";
    return failures == 0 ? 0 : 1;
}