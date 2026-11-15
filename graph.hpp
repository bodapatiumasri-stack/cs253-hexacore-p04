#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include <utility>
using namespace std;
struct Edge{
    int to;
    int id;
};

struct Graph{
    int n=0;
    int m=0;
    vector<pair<int,int>> edges;
    vector<vector<Edge>> adj;
};

#endif