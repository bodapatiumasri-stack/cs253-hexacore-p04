#include <iostream>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <random>
#include <unordered_set>
using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 4) { cerr << "Usage: ./random n m seed\n"; return 1; }
    int n = atoi(argv[1]);
    long long m = atoll(argv[2]);
    unsigned int seed = (unsigned int)atoll(argv[3]);

    long long maxEdges = (long long)n * (n - 1) / 2;
    if (n < 2 || m < 0 || m > maxEdges / 2) {
        cerr << "need n >= 2 and 0 <= m <= n*(n-1)/4\n";
        return 1;
    }

    mt19937 rng(seed);

    vector<int> label(n + 1);                    
    for (int i = 0; i <= n; i++) label[i] = i;
    shuffle(label.begin() + 1, label.end(), rng);

    unordered_set<long long> used;                
    used.reserve((size_t)m * 2 + 1);
    vector<pair<int,int>> edges;
    edges.reserve((size_t)m);
    uniform_int_distribution<int> pick(1, n);

    while ((long long)edges.size() < m) {
        int u = pick(rng), v = pick(rng);
        if (u == v) continue;                     
        if (u > v) swap(u, v);
        long long key = (long long)u * (n + 1) + v;
        if (!used.insert(key).second) continue;   
        edges.push_back(make_pair(label[u], label[v]));
    }

    shuffle(edges.begin(), edges.end(), rng);     

    cout << n << " " << m << "\n";
    for (size_t i = 0; i < edges.size(); i++) {
        int a = edges[i].first, b = edges[i].second;
        if (rng() % 2) swap(a, b);                
        cout << a << " " << b << "\n";
    }
    return 0;
}