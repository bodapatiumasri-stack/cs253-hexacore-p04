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

static StructureAResult callStructureA(const Graph& adj, int n) { return analyzeStructureA(adj, n); }
static StructureBResult callStructureB(const Graph& adj, int n) { return computeStructureB(adj, n); }

static int failures = 0;

struct TestGraph {
    int n = 0;
    vector<pair<int, int>> edges;
    Graph adj;
};

static TestGraph makeGraph(int n, const vector<pair<int, int>>& edgeList) {
    TestGraph t;
    t.n = n;
    t.edges = edgeList;
    t.adj.assign(n + 1, {});
    for (int id = 0; id < (int)edgeList.size(); id++) {
        int u = edgeList[id].first;
        int v = edgeList[id].second;
        t.adj[u].push_back({v, id});
        if (u != v) t.adj[v].push_back({u, id});
    }
    return t;
}

static vector<int> sorted(vector<int> v) {
    sort(v.begin(), v.end());
    v.erase(unique(v.begin(), v.end()), v.end());
    return v;
}

static void runNamed(const string& name, const TestGraph& t) {
    cout << "### " << name << "\n";
    StructureAResult a = callStructureA(t.adj, t.n);
    StructureBResult b = callStructureB(t.adj, t.n);
    bool ok = runOracleReport(t.adj, a.articulationPoints, a.bridges, b.bridges);
    if (!ok) failures++;
}

static bool quietCheck(const TestGraph& t, const string& label) {
    StructureAResult a = callStructureA(t.adj, t.n);
    StructureBResult b = callStructureB(t.adj, t.n);

    OracleResult o = runOracle(t.adj);

    bool ok = sorted(a.articulationPoints) == o.articulationPoints &&
              sorted(a.bridges) == o.bridges &&
              sorted(b.bridges) == o.bridges;

    if (!ok) {
        cout << "!!! MISMATCH on " << label << " (n=" << t.n
             << ", m=" << t.edges.size() << ")\n";
        cout << "Edges (id: u-v):";
        for (int i = 0; i < (int)t.edges.size(); i++) {
            cout << " " << i << ":" << t.edges[i].first << "-" << t.edges[i].second;
        }
        cout << "\n";
        runOracleReport(t.adj, a.articulationPoints, a.bridges, b.bridges);
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

static TestGraph randomGraph(int n, int m, bool allowExtras) {
    set<pair<int, int>> used;
    vector<pair<int, int>> edgeList;
    if (!allowExtras) {
        int maxEdges = n * (n - 1) / 2;
        if (m > maxEdges) m = maxEdges;
    }
    while ((int)edgeList.size() < m) {
        int u = (int)(nextRand() % n) + 1;
        int v = (int)(nextRand() % n) + 1;
        if (!allowExtras) {
            if (u == v) continue;
            pair<int, int> key = {min(u, v), max(u, v)};
            if (used.count(key)) continue;
            used.insert(key);
        }
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
    runNamed("Triangle with tail", makeGraph(4, {{1, 2}, {2, 3}, {3, 1}, {3, 4}}));
    runNamed("Two triangles joined by a path",
             makeGraph(7, {{1, 2}, {2, 3}, {3, 1}, {3, 4}, {4, 5}, {5, 6}, {6, 7}, {7, 5}}));
    runNamed("Parallel edges", makeGraph(3, {{1, 2}, {1, 2}, {2, 3}}));
    runNamed("Self-loop in the middle of a path", makeGraph(3, {{1, 2}, {2, 2}, {2, 3}}));
    runNamed("Vertex with only a self-loop", makeGraph(3, {{1, 2}, {3, 3}}));

    const int TRIALS = 500;
    int bad = 0;
    for (int t = 0; t < TRIALS; t++) {
        bool extras = (t % 2 == 1);
        int n = (int)(nextRand() % 25) + 1;
        int maxEdges = extras ? 40 : min(n * (n - 1) / 2, 40);
        int m = maxEdges == 0 ? 0 : (int)(nextRand() % (maxEdges + 1));
        TestGraph g = randomGraph(n, m, extras);
        if (!quietCheck(g, "random trial " + to_string(t))) bad++;
    }
    cout << "Random stress test: " << (TRIALS - bad) << "/" << TRIALS << " graphs matched.\n";
    failures += bad;

    cout << "\n" << (failures == 0 ? "ALL TESTS PASSED" : "SOME CHECKS FAILED")
         << " (failures: " << failures << ")\n";
    return failures == 0 ? 0 : 1;
}