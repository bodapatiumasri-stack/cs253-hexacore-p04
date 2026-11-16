#include <algorithm>
#include <iostream>
#include <random>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "../graph.hpp"
#include "../src/structA/structA.hpp"
#include "../src/structB/structB.hpp"
#include "oracle.hpp"

using namespace std;

static StructureAResult callA(const Graph& g) {
    return analyzeStructureA(g);               
}

static vector<int> callBBridges(const Graph& g) {
    return computeStructureB(g, g.n).bridges;   
}


typedef vector<pair<int, int>> EdgeList;

static int failures = 0;

static Graph makeGraph(int n, const EdgeList& edges) {
    Graph g(n);
    for (const auto& e : edges) g.addEdge(e.first, e.second);
    return g;
}

static vector<int> sorted(vector<int> v) {
    sort(v.begin(), v.end());
    v.erase(unique(v.begin(), v.end()), v.end());
    return v;
}

static void runNamed(const string& name, const Graph& g) {
    cout << "### " << name << "\n";
    StructureAResult a = callA(g);
    vector<int> b = callBBridges(g);
    if (!runOracleReport(g, a.articulationPoints, a.bridges, b)) failures++;
}

static bool quietMatch(const Graph& g) {
    StructureAResult a = callA(g);
    vector<int> b = callBBridges(g);
    OracleResult r = runOracle(g);
    return sorted(a.articulationPoints) == r.articulationPoints &&
           sorted(a.bridges) == r.bridges &&
           sorted(b) == r.bridges;
}

int main() {

    runNamed("Path", makeGraph(5, {{1,2},{2,3},{3,4},{4,5}}));
    runNamed("Cycle", makeGraph(4, {{1,2},{2,3},{3,4},{4,1}}));
    runNamed("Star", makeGraph(5, {{1,2},{1,3},{1,4},{1,5}}));
    runNamed("Disconnected + isolated vertex", makeGraph(6, {{1,2},{2,3},{4,5}}));
    runNamed("Single vertex", makeGraph(1, {}));
    runNamed("Triangle with tail", makeGraph(4, {{1,2},{2,3},{3,1},{3,4}}));
    runNamed("Two triangles joined by a path",
             makeGraph(7, {{1,2},{2,3},{3,1},{3,4},{4,5},{5,6},{6,7},{7,5}}));
    runNamed("Star centre is not vertex 1 (own DFS tree)",
             makeGraph(7, {{1,2},{3,4},{3,5},{3,6},{3,7}}));

    mt19937 rng(12345);   
    const int TRIALS = 500;
    int bad = 0;
    for (int t = 0; t < TRIALS; t++) {
        int n = rng() % 25 + 1;
        int maxEdges = min(n * (n - 1) / 2, 40);
        int m = maxEdges == 0 ? 0 : rng() % (maxEdges + 1);

        set<pair<int, int>> used;
        EdgeList edges;
        while ((int)edges.size() < m) {
            int u = rng() % n + 1, v = rng() % n + 1;
            if (u == v || used.count({min(u, v), max(u, v)})) continue;
            used.insert({min(u, v), max(u, v)});
            edges.push_back({u, v});
        }

        Graph g = makeGraph(n, edges);
        if (!quietMatch(g)) {
            bad++;
            cout << "!!! MISMATCH on random trial " << t << " (n=" << n << ", edges:";
            for (int i = 0; i < (int)edges.size(); i++)
                cout << " " << i << ":" << edges[i].first << "-" << edges[i].second;
            cout << ")\n";
            runNamed("failing random graph", g);
        }
    }
    cout << "Random stress test: " << (TRIALS - bad) << "/" << TRIALS << " graphs matched.\n";
    failures += bad;

    {
        int n = 200000;
        EdgeList edges;
        for (int i = 1; i < n; i++) edges.push_back({i, i + 1});
        Graph g = makeGraph(n, edges);
        bool ok = (int)callA(g).bridges.size() == n - 1 &&
                  (int)callBBridges(g).size() == n - 1;
        cout << "Long path (" << n << " vertices): " << (ok ? "PASS" : "FAIL") << "\n";
        if (!ok) failures++;
    }

    cout << "\n" << (failures == 0 ? "ALL TESTS PASSED" : "SOME CHECKS FAILED")
         << " (failures: " << failures << ")\n";
    return failures == 0 ? 0 : 1;
}