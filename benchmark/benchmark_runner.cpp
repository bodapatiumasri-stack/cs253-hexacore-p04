#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include <string>
#include <algorithm>

#include "../graph.hpp"
using Graph = GraphData; // Bridges structB.hpp expecting "Graph" instead of "GraphData"

#include "../src/structA/structA.hpp"
#include "../src/structB/structB.hpp"
#include "../src/oracle/oracle.hpp"

GraphData load_graph(const std::string& filepath) {
    GraphData g;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filepath << std::endl;
        return g;
    }

    if (!(file >> g.n >> g.m)) {
        std::cerr << "Error reading header (n, m) from: " << filepath << std::endl;
        return g;
    }

    g.edges.reserve(g.m);
    g.adj.assign(g.n + 1, {}); // 1-indexed: valid vertices 1..n

    for (int id = 0; id < g.m; ++id) {
        int u, v;
        if (!(file >> u >> v)) {
            std::cerr << "Error reading edge " << id << " from: " << filepath << std::endl;
            break;
        }
        g.edges.push_back({u, v});
        g.adj[u].push_back({v, id});
        g.adj[v].push_back({u, id});
    }
    return g;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: ./runner <path_to_graph_file>" << std::endl;
        return 1;
    }

    std::string filepath = argv[1];
    GraphData g = load_graph(filepath);
    if (g.n <= 0) return 1;

    // Write CSV header if results.csv does not exist or is empty
    std::ifstream check_csv("benchmark/results.csv");
    bool write_header = !check_csv.good() || check_csv.peek() == std::ifstream::traits_type::eof();
    check_csv.close();

    std::ofstream csv_out("benchmark/results.csv", std::ios::app);
    if (write_header) {
        csv_out << "Graph_Path,V,E,StructA_ms,StructB_ms,Oracle_ms,Verified\n";
    }

    // --- 1. Benchmark Structure A (Iterative Tarjan DFS) ---
    auto start_A = std::chrono::high_resolution_clock::now();
    StructureAResult resA = analyzeStructureA(g, g.n);
    auto end_A = std::chrono::high_resolution_clock::now();
    double time_A = std::chrono::duration<double, std::milli>(end_A - start_A).count();

    // --- 2. Benchmark Structure B (Union-Find Path Marking) ---
    auto start_B = std::chrono::high_resolution_clock::now();
    StructureBResult resB = computeStructureB(g, g.n);
    auto end_B = std::chrono::high_resolution_clock::now();
    double time_B = std::chrono::duration<double, std::milli>(end_B - start_B).count();

    // --- 3. Benchmark Oracle & Verify Correctness (only on small graphs) ---
    double time_Oracle = -1.0;
    std::string verified = "SKIPPED";

    if (!oracleTooLarge(g)) {
        auto start_O = std::chrono::high_resolution_clock::now();
        std::vector<int> oracle_ap = oracleArticulationPoints(g);
        std::vector<int> oracle_br = oracleBridges(g);
        auto end_O = std::chrono::high_resolution_clock::now();
        time_Oracle = std::chrono::duration<double, std::milli>(end_O - start_O).count();

        std::sort(resA.articulationPoints.begin(), resA.articulationPoints.end());
        std::sort(resA.bridges.begin(), resA.bridges.end());
        std::sort(resB.bridges.begin(), resB.bridges.end());

        bool match = (resA.articulationPoints == oracle_ap) &&
                     (resA.bridges == oracle_br) &&
                     (resB.bridges == oracle_br);

        verified = match ? "PASS" : "MISMATCH";

        if (!match) {
            std::cerr << "[WARNING] Correctness mismatch on " << filepath << "!\n";
        }
    }

    csv_out << filepath << "," << g.n << "," << g.m << ","
            << time_A << "," << time_B << "," << time_Oracle << "," << verified << "\n";
    csv_out.close();

    std::cout << "Benchmarked " << filepath
              << " (V=" << g.n << ", E=" << g.m << ")"
              << " | StructA: " << time_A << " ms"
              << " | StructB: " << time_B << " ms"
              << " | Oracle: " << (time_Oracle < 0 ? "SKIPPED" : std::to_string(time_Oracle) + " ms")
              << " | Check: " << verified << std::endl;

    return 0;
}
