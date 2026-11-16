#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "../graph.hpp"
#include "../src/structA/structA.hpp"
#include "../src/structB/structB.hpp"
#include "../app/block_cut.h"
#include "oracle.hpp"

using namespace std;

static int failures = 0;
static int d3Failures = 0;

struct TestGraph {
    Graph graph;
};

static TestGraph makeGraph(
    int n,
    const vector<pair<int, int>>& edgeList
) {
    TestGraph t;
    t.graph = Graph(n);

    for (auto edge : edgeList) {
        t.graph.addEdge(edge.first, edge.second);
    }

    return t;
}

static vector<int> sorted(vector<int> v) {
    sort(v.begin(), v.end());
    v.erase(unique(v.begin(), v.end()), v.end());
    return v;
}

static void runNamed(
    const string& name,
    const TestGraph& t
) {
    cout << "### " << name << "\n";

    StructureAResult a = analyzeStructureA(t.graph);
    StructureBResult b = computeStructureB(t.graph, t.graph.n);

    bool ok = runOracleReport(
        t.graph,
        a.articulationPoints,
        a.bridges,
        b.bridges
    );

    if (!ok) failures++;
}

// Independent D3 verification using Oracle component labels.
static bool checkD3(
    const TestGraph& t,
    const string& label,
    bool verbose = false
) {
    StructureAResult a = analyzeStructureA(t.graph);

    BlockCutForest forest =
        buildBlockCutForest(t.graph, a);

    OracleResult oracle = runOracle(t.graph);

    int n = t.graph.n;

    for (int failed = 1; failed <= n; failed++) {
        vector<int> componentLabel(n + 1, -1);

        int labelId = 0;

        // Reconstruct connected components after removing failed.
        vector<bool> visited(n + 1, false);

        for (int start = 1; start <= n; start++) {
            if (start == failed || visited[start]) {
                continue;
            }

            vector<int> stack;
            stack.push_back(start);
            visited[start] = true;

            while (!stack.empty()) {
                int u = stack.back();
                stack.pop_back();

                componentLabel[u] = labelId;

                for (const Edge& e : t.graph.adj[u]) {
                    int v = e.to;

                    if (v == failed || visited[v]) {
                        continue;
                    }

                    visited[v] = true;
                    stack.push_back(v);
                }
            }

            labelId++;
        }

        // Verify all possible endpoint pairs.
        for (int x = 1; x <= n; x++) {
            for (int y = 1; y <= n; y++) {

                bool expected;

                if (x == failed || y == failed) {
                    expected = true;
                }
                else {
                    expected =
                        componentLabel[x] != componentLabel[y];
                }

                bool actual = isDisconnectedAfterFailure(
                    forest, x, y, failed
                );

                if (actual != expected) {
                    cout << "D3 MISMATCH: " << label
                         << "\nFailed vertex: " << failed
                         << ", x: " << x
                         << ", y: " << y
                         << "\nExpected: "
                         << (expected ? "DISCONNECTED" : "CONNECTED")
                         << "\nActual: "
                         << (actual ? "DISCONNECTED" : "CONNECTED")
                         << "\n";

                    return false;
                }
            }
        }
    }

    if (verbose) {
        cout << "D3 block-cut queries: MATCH\n";
    }

    return true;
}

static bool quietCheck(
    const TestGraph& t,
    const string& label
) {
    StructureAResult a = analyzeStructureA(t.graph);
    StructureBResult b = computeStructureB(t.graph, t.graph.n);

    OracleResult o = runOracle(t.graph);

    bool ok =
        sorted(a.articulationPoints) == sorted(o.articulationPoints) &&
        sorted(a.bridges) == sorted(o.bridges) &&
        sorted(b.bridges) == sorted(o.bridges);

    if (!ok) {
        cout << "!!! STRUCTURE MISMATCH on " << label << "\n";

        runOracleReport(
            t.graph,
            a.articulationPoints,
            a.bridges,
            b.bridges
        );
    }

    bool d3ok = checkD3(t, label);

    if (!d3ok) {
        d3Failures++;
    }

    return ok && d3ok;
}

static unsigned long long rngState =
    88172645463325252ULL;

static unsigned int nextRand() {
    rngState ^= rngState << 13;
    rngState ^= rngState >> 7;
    rngState ^= rngState << 17;

    return (unsigned int)(rngState >> 11);
}

static TestGraph randomGraph(
    int n,
    int m,
    bool allowExtras
) {
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

            pair<int, int> key = {
                min(u, v),
                max(u, v)
            };

            if (used.count(key)) continue;

            used.insert(key);
        }

        edgeList.push_back({u, v});
    }

    return makeGraph(n, edgeList);
}

int main() {

    runNamed(
        "Path 1-2-3-4-5",
        makeGraph(5, {{1, 2}, {2, 3}, {3, 4}, {4, 5}})
    );

    runNamed(
        "Cycle of 4",
        makeGraph(4, {{1, 2}, {2, 3}, {3, 4}, {4, 1}})
    );

    runNamed(
        "Star",
        makeGraph(5, {{1, 2}, {1, 3}, {1, 4}, {1, 5}})
    );

    runNamed(
        "Disconnected + isolated vertex",
        makeGraph(6, {{1, 2}, {2, 3}, {4, 5}})
    );

    runNamed(
        "Single vertex",
        makeGraph(1, {})
    );

    runNamed(
        "Triangle with tail",
        makeGraph(4, {{1, 2}, {2, 3}, {3, 1}, {3, 4}})
    );

    runNamed(
        "Two triangles joined by a path",
        makeGraph(7, {
            {1, 2}, {2, 3}, {3, 1},
            {3, 4}, {4, 5},
            {5, 6}, {6, 7}, {7, 5}
        })
    );

    runNamed(
        "Parallel edges",
        makeGraph(3, {{1, 2}, {1, 2}, {2, 3}})
    );

    runNamed(
        "Self-loop in the middle of a path",
        makeGraph(3, {{1, 2}, {2, 2}, {2, 3}})
    );

    runNamed(
        "Vertex with only a self-loop",
        makeGraph(3, {{1, 2}, {3, 3}})
    );

    // D3 checks for all named test graphs.
    cout << "\n========== D3 NAMED TESTS ==========\n";

    vector<pair<string, TestGraph>> namedGraphs = {
        {"Path", makeGraph(5, {{1,2},{2,3},{3,4},{4,5}})},
        {"Cycle", makeGraph(4, {{1,2},{2,3},{3,4},{4,1}})},
        {"Star", makeGraph(5, {{1,2},{1,3},{1,4},{1,5}})},
        {"Disconnected", makeGraph(6, {{1,2},{2,3},{4,5}})},
        {"Single vertex", makeGraph(1, {})},
        {"Triangle tail", makeGraph(4, {{1,2},{2,3},{3,1},{3,4}})},
        {"Parallel edges", makeGraph(3, {{1,2},{1,2},{2,3}})},
        {"Self-loop", makeGraph(3, {{1,2},{2,2},{2,3}})}
    };

    int namedD3Passed = 0;

    for (const auto& item : namedGraphs) {
        if (checkD3(item.second, item.first, true)) {
            namedD3Passed++;
        }
        else {
            failures++;
        }
    }

    const int TRIALS = 500;
    int bad = 0;

    for (int t = 0; t < TRIALS; t++) {
        bool extras = (t % 2 == 1);

        int n = (int)(nextRand() % 25) + 1;

        int maxEdges = extras
            ? 40
            : min(n * (n - 1) / 2, 40);

        int m = maxEdges == 0
            ? 0
            : (int)(nextRand() % (maxEdges + 1));

        TestGraph g = randomGraph(n, m, extras);

        if (!quietCheck(
            g,
            "random trial " + to_string(t)
        )) {
            bad++;
        }
    }

    cout << "\nStructure A/B random stress test: "
         << (TRIALS - bad)
         << "/" << TRIALS << " graphs matched.\n";

    cout << "D3 random query stress test: "
         << (TRIALS - d3Failures)
         << "/" << TRIALS << " graphs matched.\n";

    failures += bad;

    cout << "\nD3 named tests: "
         << namedD3Passed << "/"
         << namedGraphs.size() << " passed.\n";

    cout << "\n"
         << (failures == 0
             ? "ALL TESTS PASSED"
             : "SOME CHECKS FAILED")
         << " (failures: " << failures << ")\n";

    return failures == 0 ? 0 : 1;
}
