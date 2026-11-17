#include "src/structA/structA.hpp"
#include "src/structB/structB.hpp"
#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <vector>
using namespace std;

static bool fail(string& why, const string& msg) { why = msg; return false; }

static bool sortedUnique(const vector<int>& v) {
    for (size_t i = 1; i < v.size(); i++) if (v[i - 1] >= v[i]) return false;
    return true;
}

struct DSU {                                
    vector<int> p;
    explicit DSU(int n) : p(n + 1) { for (int i = 0; i <= n; i++) p[i] = i; }
    int find(int x) { while (p[x] != x) { p[x] = p[p[x]]; x = p[x]; } return x; }
    bool unite(int a, int b) { a = find(a); b = find(b); if (a == b) return false; p[a] = b; return true; }
};
bool checkInputIsSimple(const Graph& g, string& why) {
    set<pair<int,int> > seen;
    for (int id = 0; id < g.m; id++) {
        int u = g.edges[id].first, v = g.edges[id].second;
        if (u < 1 || u > g.n || v < 1 || v > g.n) return fail(why, "I0: an edge has an endpoint outside 1..n");
        if (u == v) return fail(why, "I0: self-loop found");
        if (u > v) swap(u, v);
        if (!seen.insert(make_pair(u, v)).second) return fail(why, "I0: parallel edge found");
    }
    return true;
}

bool checkStructureAInvariants(const Graph& g, const StructureAResult& A, string& why) {
  
    if (!sortedUnique(A.articulationPoints)) return fail(why, "A1: articulation points are not sorted/unique");
    for (size_t i = 0; i < A.articulationPoints.size(); i++)
        if (A.articulationPoints[i] < 1 || A.articulationPoints[i] > g.n) return fail(why, "A1: articulation point outside 1..n");
    if (!sortedUnique(A.bridges)) return fail(why, "A1: bridges are not sorted/unique");
    for (size_t i = 0; i < A.bridges.size(); i++)
        if (A.bridges[i] < 0 || A.bridges[i] >= g.m) return fail(why, "A1: bridge edge ID outside 0..m-1");

    vector<int> blockOf(g.m, -1);
    for (size_t b = 0; b < A.biconnectedComponents.size(); b++) {
        const vector<int>& blk = A.biconnectedComponents[b];
        if (blk.empty()) return fail(why, "A2: an empty block");
        for (size_t k = 0; k < blk.size(); k++) {
            int e = blk[k];
            if (e < 0 || e >= g.m) return fail(why, "A2: block contains an edge ID outside 0..m-1");
            if (blockOf[e] != -1) return fail(why, "A2: an edge is in two blocks");
            blockOf[e] = (int)b;
        }
    }
    for (int id = 0; id < g.m; id++)
        if (blockOf[id] == -1) return fail(why, "A2: an edge is in no block");
    
    vector<bool> isBridge(g.m, false);
    for (size_t i = 0; i < A.bridges.size(); i++) isBridge[A.bridges[i]] = true;
    for (size_t b = 0; b < A.biconnectedComponents.size(); b++) {
        size_t sz = A.biconnectedComponents[b].size();
        if (sz == 2) return fail(why, "A3: a block with exactly 2 edges");
        for (size_t k = 0; k < sz; k++) {
            int e = A.biconnectedComponents[b][k];
            if (sz == 1 && !isBridge[e]) return fail(why, "A3: a one-edge block that is not listed as a bridge");
            if (sz > 1 && isBridge[e]) return fail(why, "A3: a bridge inside a larger block");
        }
    }

    vector<int> blocksOfVertex(g.n + 1, 0), lastBlock(g.n + 1, -1);
    for (size_t b = 0; b < A.biconnectedComponents.size(); b++)
        for (size_t k = 0; k < A.biconnectedComponents[b].size(); k++) {
            int e = A.biconnectedComponents[b][k];
            int ends[2] = { g.edges[e].first, g.edges[e].second };
            for (int t = 0; t < 2; t++)
                if (lastBlock[ends[t]] != (int)b) { lastBlock[ends[t]] = (int)b; blocksOfVertex[ends[t]]++; }
        }
    vector<bool> isAP(g.n + 1, false);
    for (size_t i = 0; i < A.articulationPoints.size(); i++) isAP[A.articulationPoints[i]] = true;
    for (int v = 1; v <= g.n; v++)
        if ((blocksOfVertex[v] >= 2) != isAP[v]) return fail(why, "A4: articulation points differ from 'vertices in at least 2 blocks' (vertex " + to_string(v) + ")");

    vector<int> local(g.n + 1, -1);
    for (size_t b = 0; b < A.biconnectedComponents.size(); b++) {
        const vector<int>& blk = A.biconnectedComponents[b];
        vector<int> verts;
        for (size_t k = 0; k < blk.size(); k++) {
            int ends[2] = { g.edges[blk[k]].first, g.edges[blk[k]].second };
            for (int t = 0; t < 2; t++) if (local[ends[t]] == -1) { local[ends[t]] = (int)verts.size(); verts.push_back(ends[t]); }
        }
        DSU d((int)verts.size());
        int merges = 0;
        for (size_t k = 0; k < blk.size(); k++)
            if (d.unite(local[g.edges[blk[k]].first] + 1, local[g.edges[blk[k]].second] + 1)) merges++;
        for (size_t k = 0; k < verts.size(); k++) local[verts[k]] = -1;
        if (merges != (int)verts.size() - 1) return fail(why, "A5: a block is not connected");
    }
    return true;
}

bool checkStructureBInvariants(const Graph& g, const StructureBResult& B, string& why) {

    if (!sortedUnique(B.bridges)) return fail(why, "B1: bridges are not sorted/unique");
    vector<bool> isBridge(g.m, false);
    for (size_t i = 0; i < B.bridges.size(); i++) {
        if (B.bridges[i] < 0 || B.bridges[i] >= g.m) return fail(why, "B1: bridge edge ID outside 0..m-1");
        isBridge[B.bridges[i]] = true;
    }


    vector<int> compOf(g.n + 1, -1);
    for (size_t c = 0; c < B.twoEdgeConnectedComponents.size(); c++) {
        const vector<int>& comp = B.twoEdgeConnectedComponents[c];
        if (comp.empty()) return fail(why, "B2: an empty component");
        if (!sortedUnique(comp)) return fail(why, "B2: a component is not sorted/unique");
        if (c > 0 && B.twoEdgeConnectedComponents[c - 1][0] >= comp[0]) return fail(why, "B2: components are not sorted by first vertex");
        for (size_t k = 0; k < comp.size(); k++) {
            int v = comp[k];
            if (v < 1 || v > g.n) return fail(why, "B2: vertex outside 1..n in a component");
            if (compOf[v] != -1) return fail(why, "B2: a vertex is in two components");
            compOf[v] = (int)c;
        }
    }
    for (int v = 1; v <= g.n; v++)
        if (compOf[v] == -1) return fail(why, "B2: a vertex is in no component");

    for (int id = 0; id < g.m; id++) {
        int cu = compOf[g.edges[id].first], cv = compOf[g.edges[id].second];
        if (isBridge[id] && cu == cv) return fail(why, "B3: a bridge lies inside one component");
        if (!isBridge[id] && cu != cv) return fail(why, "B3: a non-bridge edge joins two components");
    }

    DSU d(g.n);
    for (int id = 0; id < g.m; id++) if (!isBridge[id]) d.unite(g.edges[id].first, g.edges[id].second);
    int pieces = 0;
    for (int v = 1; v <= g.n; v++) if (d.find(v) == v) pieces++;
    if (pieces != (int)B.twoEdgeConnectedComponents.size()) return fail(why, "B4: the number of components does not match the pieces left after removing the bridges");
    return true;
}

bool checkAandBAgree(const StructureAResult& A, const StructureBResult& B, string& why) {
    if (A.bridges != B.bridges) return fail(why, "AB: Structure A and Structure B report different bridges");
    return true;
}

static Graph readGraphForCheck(istream& in) {
    int n = 0, m = 0;
    in >> n >> m;
    Graph g(n);
    for (int i = 0; i < m; i++) { int u, v; in >> u >> v; g.addEdge(u, v); }
    return g;
}

int main(int argc, char**) {
    bool quiet = argc > 1;
    Graph g = readGraphForCheck(cin);
    string why;
    if (!checkInputIsSimple(g, why)) { cout << "INPUT NOT VALID: " << why << "\n"; return 2; }

    StructureAResult A = analyzeStructureA(g, g.n);
    StructureBResult B = computeStructureB(g, g.n);

    bool okA = checkStructureAInvariants(g, A, why);
    string whyA = why;
    bool okB = checkStructureBInvariants(g, B, why);
    string whyB = why;
    bool okAB = checkAandBAgree(A, B, why);
    string whyAB = why;

    if (!quiet) {
        cout << "n=" << g.n << " m=" << g.m << "\n";
        cout << "Structure A invariants (A1-A5): " << (okA ? "OK" : "VIOLATED - " + whyA) << "\n";
        cout << "Structure B invariants (B1-B4): " << (okB ? "OK" : "VIOLATED - " + whyB) << "\n";
        cout << "A and B bridges agree:          " << (okAB ? "OK" : "VIOLATED - " + whyAB) << "\n";
    } else {
        if (!okA) cout << whyA << "\n";
        if (!okB) cout << whyB << "\n";
        if (!okAB) cout << whyAB << "\n";
    }
    bool ok = okA && okB && okAB;
    cout << (ok ? "ALL INVARIANTS OK" : "INVARIANT VIOLATED") << "\n";
    return ok ? 0 : 1;
}
