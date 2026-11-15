#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include <utility>
using namespace std;

struct Edge {
    int to;
    int id;
};

struct Graph {
    int n = 0;
    int m = 0;
    vector<pair<int,int>> edges;    
   vector<vector<Edge>> adj;         

    explicit Graph(int n_ = 0) : n(n_), adj(n_ + 1) {}

    int addEdge(int u, int v) {
        int id = m++;
        edges.push_back({u, v});
        adj[u].push_back({v, id});
        adj[v].push_back({u, id});              
        return id;
    }
};

#endif
