#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include "graph.hpp"
#include "oracle.hpp"

using namespace std;

static int failures = 0;

static Graph buildGraph(int n, const vector<pair<int, int>>& edgeList) {
    Graph adj(n + 1);
    for (int id = 0; id < (int)edgeList.size(); id++) {
        int u = edgeList[id].first;
        int v = edgeList[id].second;
        adj[u].push_back({v, id});
        if (u != v) adj[v].push_back({u, id});
    }
    return adj;
}

static void check(const string& what, const vector<int>& actual, const vector<int>& expected) {
    if (actual == expected) {
        cout << "  PASS  " << what << "\n";
    } else {
        cout << "  FAIL  " << what << "\n";
        failures++;
    }
}

static void runCase(const string& name, int n, const vector<pair<int, int>>& edgeList,
                    int expectedComponents,
                    const vector<int>& expectedAP, const vector<int>& expectedBridges) {
    Graph adj = buildGraph(n, edgeList);
    OracleResult r = runOracle(adj);
    cout << name << "\n";
    check("original components", {r.originalComponentCount}, {expectedComponents});
    check("articulation points", r.articulationPoints, expectedAP);
    check("bridges", r.bridges, expectedBridges);
    check("countComponents(adj, n) agrees", {countComponents(adj, n)}, {expectedComponents});
}

int main() {

    runCase("1. Single vertex", 1, {}, 1, {}, {});

    runCase("2. Path", 5, {{1, 2}, {2, 3}, {3, 4}, {4, 5}},
            1, {2, 3, 4}, {0, 1, 2, 3});

    runCase("3. Cycle", 4, {{1, 2}, {2, 3}, {3, 4}, {4, 1}},
            1, {}, {});

    runCase("4. Star", 5, {{1, 2}, {1, 3}, {1, 4}, {1, 5}},
            1, {1}, {0, 1, 2, 3});

    runCase("5. Disconnected", 5, {{1, 2}, {2, 3}, {4, 5}},
            2, {2}, {0, 1, 2});

    runCase("6. Isolated vertices", 4, {{1, 2}},
            3, {}, {0});

    runCase("7. No articulation points (K4)", 4,
            {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}},
            1, {}, {});

    runCase("8. Several articulation points", 7,
            {{1, 2}, {2, 3}, {3, 1}, {3, 4}, {4, 5}, {5, 6}, {6, 7}, {7, 5}},
            1, {3, 4, 5}, {3, 4});

    runCase("9. Several bridges", 5,
            {{1, 2}, {2, 3}, {3, 1}, {3, 4}, {4, 5}},
            1, {3, 4}, {3, 4});

    runCase("10. Disconnected, original > 1", 6,
            {{1, 2}, {2, 3}, {4, 5}, {5, 6}, {6, 4}},
            2, {2}, {0, 1});

    runCase("11. Parallel edges", 3, {{1, 2}, {1, 2}, {2, 3}},
            1, {2}, {2});

    runCase("12. Self-loop on path vertex", 3, {{1, 2}, {2, 2}, {2, 3}},
            1, {2}, {0, 2});

    runCase("13. Vertex with only a self-loop", 3, {{1, 2}, {3, 3}},
            2, {}, {0});

    runCase("14. Self-loop + parallel edges", 3, {{1, 2}, {1, 2}, {1, 1}, {2, 3}},
            1, {2}, {3});

    cout << "15. Exact result arrays on the path\n";
    {
        OracleResult r = runOracle(buildGraph(5, {{1, 2}, {2, 3}, {3, 4}, {4, 5}}));
        check("componentsAfterVertexRemoval", r.componentsAfterVertexRemoval,
              {0, 1, 2, 2, 2, 1});
        check("componentsAfterEdgeRemoval", r.componentsAfterEdgeRemoval,
              {2, 2, 2, 2});
    }

    cout << "16. Removing a self-loop changes nothing\n";
    {
        Graph adj = buildGraph(3, {{1, 2}, {2, 2}, {2, 3}});
        check("count with self-loop removed", {countComponents(adj, 3, -1, 1)},
              {countComponents(adj, 3)});
    }

    cout << "\nReport demo (all MATCH):\n";
    {
        Graph adj = buildGraph(5, {{1, 2}, {2, 3}, {3, 4}, {4, 5}});
        bool ok = runOracleReport(adj, {2, 3, 4}, {0, 1, 2, 3}, {0, 1, 2, 3});
        if (!ok) failures++;
    }

    cout << "Report demo (deliberate MISMATCH):\n";
    {
        Graph adj = buildGraph(5, {{1, 2}, {2, 3}, {3, 4}, {4, 5}});
        bool ok = runOracleReport(adj, {1, 2, 3}, {1, 2, 3}, {0, 1, 2});
        if (ok) failures++;
    }

    cout << "Size guard demo:\n";
    {
        Graph adj = buildGraph(6000, {});
        runOracleReport(adj, {}, {}, {});
        check("oracleTooLarge(6000 vertices)", {oracleTooLarge(adj) ? 1 : 0}, {1});
    }

    cout << "\n" << (failures == 0 ? "ALL TESTS PASSED" : "SOME TESTS FAILED")
         << " (failures: " << failures << ")\n";
    return failures == 0 ? 0 : 1;
}