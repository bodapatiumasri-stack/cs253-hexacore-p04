#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include <string>
#include <algorithm>

#include "../graph.hpp"
using AdjList = std::vector<std::vector<Edge>>;
using GraphData = Graph;


#include "../src/structA/structA.hpp"
#include "../src/structB/structB.hpp"
#include "../oracle/oracle.hpp"

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
    g.adj.assign(g.n + 1, {}); 

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

    
    std::ifstream check_csv("benchmark/results.csv");
    bool write_header = !check_csv.good() || check_csv.peek() == std::ifstream::traits_type::eof();
    check_csv.close();

    std::ofstream csv_out("benchmark/results.csv", std::ios::app);
    if (write_header) {
        csv_out << "Graph_Path,V,E,StructA_ms,StructB_ms,Oracle_ms,Verified\n";
    }
-
    auto start_A = std::chrono::high_resolution_clock::now();
    StructureAResult resA = analyzeStructureA(g, g.n);
    auto end_A = std::chrono::high_resolution_clock::now();
    double time_A = std::chrono::duration<double, std::milli>(end_A - start_A).count();


    auto start_B = std::chrono::high_resolution_clock::now();
    StructureBResult resB = computeStructureB(g, g.n);
    auto end_B = std::chrono::high_resolution_clock::now();
    double time_B = std::chrono::duration<double, std::milli>(end_B - start_B).count();


    double time_Oracle = -1.0;
    std::string verified = "SKIPPED";

    if (!oracleTooLarge(g)) {
        auto start_O = std::chrono::high_resolution_clock::now();
        OracleResult o_res = runOracle(g); std::vector<int> oracle_ap = o_res.articulationPoints; std::vector<int> oracle_br = o_res.bridges;
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

