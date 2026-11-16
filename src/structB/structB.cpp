
#include "structB.hpp"
#include <vector>
#include <algorithm>
#include<utility>
#include<queue>

static bool isAncestor(
    int u,
    int v,
    const std::vector<int>& tin,
    const std::vector<int>& tout
) {
    return tin[u] <= tin[v] && tout[v] <= tout[u];
}

static int findJump(int x, std::vector<int>& jump) {

    int root = x;

    while (jump[root] != root) {
        root = jump[root];
    }

    while (jump[x] != x) {
        int p = jump[x];
        jump[x] = root;
        x = p;
    }

    return root;
}

static void buildDFS(
    const Graph& g,
    std::vector<int>& parent,
    std::vector<int>& parentEdge,
    std::vector<int>& depth,
    std::vector<int>& tin,
    std::vector<int>& tout
) {
    int n = g.n;

    std::vector<int> it(n + 1, 0);
    int timer = 0;

    for (int root = 1; root <= n; root++) {

        if (tin[root] != -1) {
            continue;
        }

        std::vector<int> st;

        tin[root] = timer++;
        st.push_back(root);

        while (!st.empty()) {

            int u = st.back();

            if (it[u] < (int)g.adj[u].size()) {

                Edge e = g.adj[u][it[u]++];
                int v = e.to;

                if (e.id == parentEdge[u]) {
                    continue;
                }

                if (tin[v] == -1) {

                    parent[v] = u;
                    parentEdge[v] = e.id;
                    depth[v] = depth[u] + 1;

                    tin[v] = timer++;
                    st.push_back(v);
                }
            }
            else {

                tout[u] = timer;
                st.pop_back();
            }
        }
    }
}


static std::vector<std::pair<int, int>> findBackEdges(
    const Graph& g,
    const std::vector<int>& parentEdge,
    const std::vector<int>& tin,
    const std::vector<int>& tout
) {
    std::vector<std::pair<int, int>> backEdges;

    for (int id = 0; id < g.m; id++) {

        int u = g.edges[id].first;
        int v = g.edges[id].second;

        if (parentEdge[u] == id || parentEdge[v] == id) {
            continue;
        }

        if (isAncestor(u, v, tin, tout)) {
            backEdges.push_back({v, u});
        }
        else if (isAncestor(v, u, tin, tout)) {
            backEdges.push_back({u, v});
        }
    }

    return backEdges;
}


static std::vector<bool> markCycleEdges(
    const Graph& g,
    const std::vector<int>& parent,
    const std::vector<int>& parentEdge,
    const std::vector<int>& depth,
    const std::vector<std::pair<int, int>>& backEdges
) {
    int n = g.n;
    int m = g.m;

    std::vector<int> jump(n + 1);

    for (int i = 0; i <= n; i++) {
        jump[i] = i;
    }

    std::vector<bool> marked(m, false);

    for (auto edge : backEdges) {

        int descendant = edge.first;
        int ancestor = edge.second;

        int x = findJump(descendant, jump);

        while (depth[x] > depth[ancestor]) {

            marked[parentEdge[x]] = true;

            jump[x] = findJump(parent[x], jump);

            x = findJump(x, jump);
        }
    }

    return marked;
}
static std::vector<bool> findBridges(
    int n,
    int m,
    const std::vector<int>& parentEdge,
    const std::vector<bool>& marked,
    std::vector<int>& bridges
) {
    std::vector<bool> isBridge(m, false);

    for (int v = 1; v <= n; v++) {

        if (parentEdge[v] != -1 && !marked[parentEdge[v]]) {

            int id = parentEdge[v];

            isBridge[id] = true;
            bridges.push_back(id);
        }
    }

    std::sort(bridges.begin(), bridges.end());

    return isBridge;
}
static std::vector<std::vector<int>> findTwoEdgeConnectedComponents(
    const Graph& g,
    const std::vector<bool>& isBridge
) {
    int n = g.n;

    std::vector<bool> visited(n + 1, false);

    std::vector<std::vector<int>> components;

    for (int start = 1; start <= n; start++) {

        if (visited[start]) {
            continue;
        }

        std::vector<int> component;
        std::queue<int> q;

        q.push(start);
        visited[start] = true;

        while (!q.empty()) {

            int u = q.front();
            q.pop();

            component.push_back(u);

            for (const Edge& e : g.adj[u]) {

                if (isBridge[e.id] || visited[e.to]) {
                    continue;
                }

                visited[e.to] = true;
                q.push(e.to);
            }
        }

        std::sort(component.begin(), component.end());

        components.push_back(component);
    }

    std::sort(components.begin(), components.end());

    return components;
}
StructureBResult computeStructureB(const Graph& adj,int n) {

     n = adj.n;
    int m = adj.m;

    std::vector<int> parent(n + 1, 0);
    std::vector<int> parentEdge(n + 1, -1);
    std::vector<int> depth(n + 1, 0);
    std::vector<int> tin(n + 1, -1);
    std::vector<int> tout(n + 1, -1);

    buildDFS(adj, parent, parentEdge, depth, tin, tout);

    std::vector<std::pair<int, int>> backEdges =
        findBackEdges(adj, parentEdge, tin, tout);

    std::vector<bool> marked =
        markCycleEdges(adj, parent, parentEdge, depth, backEdges);

    StructureBResult result;

    std::vector<bool> isBridge =
        findBridges(n, m, parentEdge, marked, result.bridges);

    result.twoEdgeConnectedComponents =
        findTwoEdgeConnectedComponents(adj, isBridge);

    return result;
}
