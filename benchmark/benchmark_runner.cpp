#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include <string>

// Header placeholders for teammate integrations (uncomment when ready)
// #include "../src/structure_a.h"
// #include "../src/structure_b.h"
// #include "../src/oracle.h"

struct Graph {
    int V = 0;
    int E = 0;
    std::vector<std::vector<int>> adj; // 1-indexed: valid indices 1..V
};

Graph load_graph(const std::string& filepath) {
    Graph g;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filepath << std::endl;
        return g;
    }
    file >> g.V >> g.E;
    g.adj.resize(g.V + 1); // +1 for 1-based indexing (1 to V)
    for (int i = 0; i < g.E; ++i) {
        int u, v;
        file >> u >> v;
        g.adj[u].push_back(v);
        g.adj[v].push_back(u);
    }
    return g;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: ./runner <path_to_graph_file>" << std::endl;
        return 1;
    }

    std::string filepath = argv[1];
    Graph g = load_graph(filepath);
    if (g.V == 0) return 1;

    // Check if results.csv is empty/new to write the header row automatically
    std::ifstream check_csv("benchmark/results.csv");
    bool write_header = !check_csv.good() || check_csv.peek() == std::ifstream::traits_type::eof();
    check_csv.close();

    std::ofstream csv_out("benchmark/results.csv", std::ios::app);
    if (write_header) {
        csv_out << "Graph_Path,V,E,StructA_ms,StructB_ms,Oracle_ms\n";
    }

    // --- Benchmark Structure A ---
    auto start_A = std::chrono::high_resolution_clock::now();
    // run_structure_a(g.V, g.adj);
    auto end_A = std::chrono::high_resolution_clock::now();
    double time_A = std::chrono::duration<double, std::milli>(end_A - start_A).count();

    // --- Benchmark Structure B ---
    auto start_B = std::chrono::high_resolution_clock::now();
    // run_structure_b(g.V, g.adj);
    auto end_B = std::chrono::high_resolution_clock::now();
    double time_B = std::chrono::duration<double, std::milli>(end_B - start_B).count();

    // --- Benchmark Oracle (Only run if V <= 5000) ---
    double time_Oracle = -1.0;
    if (g.V <= 5000) {
        auto start_O = std::chrono::high_resolution_clock::now();
        // run_oracle(g.V, g.adj);
        auto end_O = std::chrono::high_resolution_clock::now();
        time_Oracle = std::chrono::duration<double, std::milli>(end_O - start_O).count();
    }

    csv_out << filepath << "," << g.V << "," << g.E << "," 
            << time_A << "," << time_B << "," << time_Oracle << "\n";

    csv_out.close();
    std::cout << "Benchmarked " << filepath << " (V=" << g.V << ", E=" << g.E << ") successfully." << std::endl;
    return 0;
}