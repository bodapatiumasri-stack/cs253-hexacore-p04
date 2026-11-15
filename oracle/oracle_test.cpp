#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "graph.hpp"
#include "oracle.hpp"

using namespace std;

static int failures = 0;

typedef vector<pair<int, int>> EdgeList;

static Graph make(int n, const EdgeList& edges) {
    Graph g(n);
    for (const auto& e : edges) g.addEdge(e.first, e.second);
    return g;
}

static void check(const string& what, bool ok) {
    if (!ok) {
        cout << "  FAIL  " << what << "\n";
        failures++;
    }
}

static void runCase(const string& name, int n, const EdgeList& edges,
                    int expectedComponents,
                    const vector<int>& expectedAP,
                    const vector<int>& expectedBridges) {
    Graph g = make(n, edges);
    OracleResult r = runOracle(g);

    int before = failures;
    check("original components", r.originalComponentCount == expectedComponents);
    check("articulation points", r.articulationPoints == expectedAP);
    check("bridges", r.bridges == expectedBridges);
    check("countComponents(g.adj, g.n) agrees",
          countComponents(g.adj, g.n) == r.originalComponentCount);
    cout << (failures == before ? "PASS  " : "FAIL  ") << name << "\n";
}

int main() {
    runCase("1. Single vertex", 1, {}, 1, {}, {});
    runCase("2. Path", 5, {{1,2},{2,3},{3,4},{4,5}}, 1, {2,3,4}, {0,1,2,3});
    runCase("3. Cycle", 4, {{1,2},{2,3},{3,4},{4,1}}, 1, {}, {});
    runCase("4. Star", 5, {{1,2},{1,3},{1,4},{1,5}}, 1, {1}, {0,1,2,3});
    runCase("5. Disconnected", 5, {{1,2},{2,3},{4,5}}, 2, {2}, {0,1,2});
    runCase("6. Isolated vertices", 4, {{1,2}}, 3, {}, {0});
    runCase("7. K4 (no articulation points)", 4,
            {{1,2},{1,3},{1,4},{2,3},{2,4},{3,4}}, 1, {}, {});
    runCase("8. Two triangles joined by a path", 7,
            {{1,2},{2,3},{3,1},{3,4},{4,5},{5,6},{6,7},{7,5}}, 1, {3,4,5}, {3,4});
    runCase("9. Triangle with two tails", 5,
            {{1,2},{2,3},{3,1},{3,4},{4,5}}, 1, {3,4}, {3,4});
    runCase("10. Disconnected, original > 1", 6,
            {{1,2},{2,3},{4,5},{5,6},{6,4}}, 2, {2}, {0,1});
    runCase("11. Parallel edges", 3, {{1,2},{1,2},{2,3}}, 1, {2}, {2});
    runCase("12. Self-loop on a path vertex", 3, {{1,2},{2,2},{2,3}}, 1, {2}, {0,2});
    runCase("13. Vertex with only a self-loop", 2, {{1,1},{2,2}}, 2, {}, {});

    {
        Graph g = make(5, {{1,2},{2,3},{3,4},{4,5}});
        OracleResult r = runOracle(g);
        int before = failures;
        check("after vertex removal", r.componentsAfterVertexRemoval ==
              vector<int>({0, 1, 2, 2, 2, 1}));
        check("after edge removal", r.componentsAfterEdgeRemoval ==
              vector<int>({2, 2, 2, 2}));
        cout << (failures == before ? "PASS  " : "FAIL  ") << "14. Exact result arrays on the path\n";
    }

    {
        Graph big(6000);
        int before = failures;
        check("oracleTooLarge(6000 vertices)", oracleTooLarge(big));
        check("small graph is not too large", !oracleTooLarge(make(5, {{1,2}})));
        cout << (failures == before ? "PASS  " : "FAIL  ") << "15. Size guard\n";
    }

    {
        int before = failures;
        cout << "-- compareResults demo (MISMATCH lines are expected) --\n";
        check("same lists match", compareResults("demo same", {3, 1, 2}, {1, 2, 3, 3}));
        check("different lists do not match", !compareResults("demo different", {1, 2}, {2, 5}));
        cout << (failures == before ? "PASS  " : "FAIL  ") << "16. compareResults\n";
    }

    cout << "\n" << (failures == 0 ? "ALL TESTS PASSED" : "SOME TESTS FAILED")
         << " (failures: " << failures << ")\n";
    return failures == 0 ? 0 : 1;
}
