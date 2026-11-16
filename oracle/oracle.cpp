#include "oracle.hpp"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <queue>
using namespace std;

int countComponents(const AdjList& adj, int n, int removedVertex, int removedEdge) {
    vector<bool> visited(n + 1, false);
    int count = 0;

    for (int start = 1; start <= n; start++) {
        if (start == removedVertex) continue;
        if (visited[start]) continue;

        count++;
        queue<int> q;
        q.push(start);
        visited[start] = true;

        while (!q.empty()) {
            int u = q.front();
            q.pop();

            for (const Edge& e : adj[u]) {
                if (e.id == removedEdge) continue;
                int v = e.to;
                if (v == removedVertex) continue;
                if (!visited[v]) {
                    visited[v] = true;
                    q.push(v);
                }
            }
        }
    }
    return count;
}

OracleResult runOracle(const Graph& graph) {
    int n = graph.n;
    int m = graph.m;

    OracleResult r;
    r.originalComponentCount = countComponents(graph.adj, n);

    r.componentsAfterVertexRemoval.assign(n + 1, 0);
    for (int v = 1; v <= n; v++) {
        int after = countComponents(graph.adj, n, v, -1);
        r.componentsAfterVertexRemoval[v] = after;
        if (after > r.originalComponentCount) {
            r.articulationPoints.push_back(v);
        }
    }

    r.componentsAfterEdgeRemoval.assign(m, 0);
    for (int id = 0; id < m; id++) {
        int after = countComponents(graph.adj, n, -1, id);
        r.componentsAfterEdgeRemoval[id] = after;
        if (after > r.originalComponentCount) {
            r.bridges.push_back(id);
        }
    }
    return r;
}

vector<int> oracleArticulationPoints(const Graph& graph) {
    int original = countComponents(graph.adj, graph.n);
    vector<int> result;
    for (int v = 1; v <= graph.n; v++) {
        if (countComponents(graph.adj, graph.n, v, -1) > original) {
            result.push_back(v);
        }
    }
    return result;
}

vector<int> oracleBridges(const Graph& graph) {
    int original = countComponents(graph.adj, graph.n);
    vector<int> result;
    for (int id = 0; id < graph.m; id++) {
        if (countComponents(graph.adj, graph.n, -1, id) > original) {
            result.push_back(id);
        }
    }
    return result;
}

bool oracleTooLarge(const Graph& graph) {
    return graph.n > 5000 || graph.m > 25000;
}

static void printList(const vector<int>& v) {
    if (v.empty()) {
        cout << "(none)";
        return;
    }
    for (size_t i = 0; i < v.size(); i++) {
        if (i > 0) cout << " ";
        cout << v[i];
    }
}

bool compareResults(const string& label, vector<int> oracle, vector<int> other) {
    sort(oracle.begin(), oracle.end());
    sort(other.begin(), other.end());
    oracle.erase(unique(oracle.begin(), oracle.end()), oracle.end());
    other.erase(unique(other.begin(), other.end()), other.end());

    if (oracle == other) {
        cout << label << ": MATCH\n";
        return true;
    }

    vector<int> missing, extra;
    set_difference(oracle.begin(), oracle.end(), other.begin(), other.end(),
                   back_inserter(missing));
    set_difference(other.begin(), other.end(), oracle.begin(), oracle.end(),
                   back_inserter(extra));

    cout << label << ": MISMATCH\n";
    cout << "    Missing (Oracle found, other did not): ";
    printList(missing);
    cout << "\n    Extra   (other reported, Oracle did not): ";
    printList(extra);
    cout << "\n";
    return false;
}

bool runOracleReport(const Graph& graph,
                     const vector<int>& structureA_ArticulationPoints,
                     const vector<int>& structureA_Bridges,
                     const vector<int>& structureB_Bridges) {
    if (oracleTooLarge(graph)) {
        cout << "Oracle skipped: input too large.\n";
        return false;
    }

    OracleResult r = runOracle(graph);

    cout << "========== ORACLE RESULTS ==========\n\n";
    cout << "Original components: " << r.originalComponentCount << "\n\n";
    cout << "Articulation Points:\n";
    printList(r.articulationPoints);
    cout << "\n\nBridges (Edge IDs):\n";
    printList(r.bridges);
    cout << "\n\n========== VERIFICATION ==========\n\n";

    bool ok = true;
    ok &= compareResults("Structure A - Articulation Points", r.articulationPoints,
                         structureA_ArticulationPoints);
    ok &= compareResults("Structure A - Bridges", r.bridges, structureA_Bridges);
    ok &= compareResults("Structure B - Bridges", r.bridges, structureB_Bridges);

    cout << "\n===================================\n\n";
    return ok;
}