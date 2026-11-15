#include "oracle.hpp"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <queue>
using namespace std;
int countComponents(const GraphData& g, int removedVertex, int removedEdge) {
    vector<bool> visited(g.n + 1, false);   
    int count = 0;

    for (int start = 1; start <= g.n; start++) {
        if (start == removedVertex) continue;   
        if (visited[start]) continue;          

        count++;                               
        queue<int> q;
        q.push(start);
        visited[start] = true;

        while (!q.empty()) {
            int u = q.front();
            q.pop();

            for (const Edge& e : g.adj[u]) {
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

vector<int> oracleArticulationPoints(const GraphData& g) {
    int original = countComponents(g);
    vector<int> result;

    for (int v = 1; v <= g.n; v++) {
        if (countComponents(g, v, -1) > original) {
            result.push_back(v);
        }
    }
    return result;
}

vector<int> oracleBridges(const GraphData& g) {
    int original = countComponents(g);
    vector<int> result;

    for (int id = 0; id < g.m; id++) {
        if (countComponents(g, -1, id) > original) {
            result.push_back(id);
        }
    }
    return result;
}

bool oracleTooLarge(const GraphData& g) {
    return g.n > 5000 || g.m > 25000;
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

bool runOracleReport(const GraphData& g,
                     const vector<int>& structureA_ArticulationPoints,
                     const vector<int>& structureA_Bridges,
                     const vector<int>& structureB_Bridges) {
    if (oracleTooLarge(g)) {
        cout << "Oracle skipped: input too large.\n";
        return false;
    }

    int original = countComponents(g);
    vector<int> ap = oracleArticulationPoints(g);
    vector<int> br = oracleBridges(g);

    cout << "========== ORACLE RESULTS ==========\n\n";
    cout << "Original components: " << original << "\n\n";
    cout << "Articulation Points:\n";
    printList(ap);
    cout << "\n\nBridges (Edge IDs):\n";
    printList(br);
    cout << "\n\n========== VERIFICATION ==========\n\n";

    bool ok = true;
    ok &= compareResults("Structure A - Articulation Points", ap, structureA_ArticulationPoints);
    ok &= compareResults("Structure A - Bridges", br, structureA_Bridges);
    ok &= compareResults("Structure B - Bridges", br, structureB_Bridges);

    cout << "\n===================================\n\n";
    return ok;
}