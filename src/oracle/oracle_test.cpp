#include <iostream>
#include <string>
#include <vector>
#include "graph.hpp"
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

static void check(const string& what, const vector<int>& actual, const vector<int>& expected) {
    if (actual == expected) {
        cout << "  PASS  " << what << "\n";
    } else {
        cout << "  FAIL  " << what << "\n";
        failures++;
    }
}

static void runCase(const string& name, const GraphData& g, int expectedComponents,
                    const vector<int>& expectedAP, const vector<int>& expectedBridges) {
    cout << name << "\n";
    check("original components", {countComponents(g)}, {expectedComponents});
    check("articulation points", oracleArticulationPoints(g), expectedAP);
    check("bridges", oracleBridges(g), expectedBridges);
}

int main() {
    runCase("1. Single vertex", makeGraph(1, {}), 1, {}, {});

    runCase("2. Path", makeGraph(5, {{1, 2}, {2, 3}, {3, 4}, {4, 5}}),
            1, {2, 3, 4}, {0, 1, 2, 3});

    runCase("3. Cycle", makeGraph(4, {{1, 2}, {2, 3}, {3, 4}, {4, 1}}),
            1, {}, {});

    runCase("4. Star", makeGraph(5, {{1, 2}, {1, 3}, {1, 4}, {1, 5}}),
            1, {1}, {0, 1, 2, 3});

    runCase("5. Disconnected", makeGraph(5, {{1, 2}, {2, 3}, {4, 5}}),
            2, {2}, {0, 1, 2});

    runCase("6. Isolated vertices", makeGraph(4, {{1, 2}}),
            3, {}, {0});

    runCase("7. No articulation points (K4)",
            makeGraph(4, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}}),
            1, {}, {});

    runCase("8. Several articulation points",
            makeGraph(7, {{1, 2}, {2, 3}, {3, 1}, {3, 4}, {4, 5}, {5, 6}, {6, 7}, {7, 5}}),
            1, {3, 4, 5}, {3, 4});

    runCase("9. Several bridges",
            makeGraph(5, {{1, 2}, {2, 3}, {3, 1}, {3, 4}, {4, 5}}),
            1, {3, 4}, {3, 4});

    runCase("10. Disconnected, original > 1",
            makeGraph(6, {{1, 2}, {2, 3}, {4, 5}, {5, 6}, {6, 4}}),
            2, {2}, {0, 1});

    runCase("11. (extra) Parallel edges",
            makeGraph(3, {{1, 2}, {1, 2}, {2, 3}}),
            1, {2}, {2});

    cout << "\nReport demo (all MATCH):\n";
    {
        GraphData g = makeGraph(5, {{1, 2}, {2, 3}, {3, 4}, {4, 5}});
        bool ok = runOracleReport(g, {2, 3, 4}, {0, 1, 2, 3}, {0, 1, 2, 3});
        if (!ok) failures++;
    }

    cout << "Report demo (deliberate MISMATCH):\n";
    {
        GraphData g = makeGraph(5, {{1, 2}, {2, 3}, {3, 4}, {4, 5}});
        bool ok = runOracleReport(g, {1, 2, 3}, {1, 2, 3}, {0, 1, 2});
        if (ok) failures++;   // must NOT report all-match
    }

    cout << "Size guard demo:\n";
    {
        GraphData g = makeGraph(6000, {});
        runOracleReport(g, {}, {}, {});
        check("oracleTooLarge(6000 vertices)", {oracleTooLarge(g) ? 1 : 0}, {1});
    }

    cout << "\n" << (failures == 0 ? "ALL TESTS PASSED" : "SOME TESTS FAILED")
         << " (failures: " << failures << ")\n";
    return failures == 0 ? 0 : 1;
}