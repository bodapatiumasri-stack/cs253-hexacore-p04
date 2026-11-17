# Project 04: Critical nodes and links in a network
Team hexacore — CS 253 / MA 253, [Autumn 2026]

## Team and roles

| Name | Roll no. | GitHub | Role | Module folder | Derivation |
|------|----------|--------|------|---------------|------------|
| Rishika Goyal | 250008029 | @che250008029-netizen | Structure A owner | src/structA/ | D1 |
| Bodapati Uma Sri | 250041009 | @bodapatiumasri-stack | Structure B owner | src/structB/ | D2 |
| Deepali Chauhan | 250041014 | @DeepaliChauhan07 | Architect | app/ | D3 |
| Keetha Akshaya | 250041021 | @akshaya129 | Oracle owner | oracle/ | D4 |
| Dharavath Mamatha | 250041015 | @mamathadharavath35-a11y | Adversary | adversary/ | D5 |
| Kunreddy Sahasra Reddy | 250041023 | @sahasrakunreddy | Analyst | benchmark/, docs/ | D6 |

## What the Project Does
This project identifies critical vertices (articulation points), critical links (bridges), and biconnected components in undirected multigraphs in $O(V + E)$ time, constructs a Block-Cut Forest to rank node criticality and answer single-vertex failure disconnection queries, and validates correctness and scaling against a brute-force Oracle across adversarial topologies up to $V = 100{,}000$.

## How to Build It
From the repository root, compile all executables with `g++ -O2 -std=c++17`:

```bash
g++ -O2 -std=c++17 -I. src/structA/structA.cpp src/structB/structB.cpp app/block_cut.cpp oracle/oracle.cpp oracle/oracle_integration_test.cpp -o integration_test && \
g++ -O2 -std=c++17 -I. src/structA/structA.cpp src/structB/structB.cpp oracle/oracle.cpp oracle/invariants_check.cpp -o invariants_check && \
g++ -O2 -std=c++17 -I. src/structA/structA.cpp src/structB/structB.cpp app/block_cut.cpp app/ranking.cpp app/main.cpp -o app_demo && \
g++ -O2 -std=c++17 -I. src/structA/structA.cpp src/structB/structB.cpp oracle/oracle.cpp benchmark/benchmark_runner.cpp -o benchmark_runner && \
for f in adversary/*.cpp; do g++ -O2 -std=c++17 "$f" -o "gen_$(basename "${f%.cpp}")"; done

One Command to Run Tests, Application, and Every BenchmarkRun the differential integration tests, structural invariant checks, interactive Block-Cut Forest application, and full-scale ($V = 100{,}000$) benchmarks across every adversarial topology in a single command:

./integration_test && \
printf "5 5\n1 2\n2 3\n3 1\n3 4\n4 5\n3\n1 5 3\n1 5 4\n1 2 3\n" | ./app_demo && \
for gen in ./gen_path ./gen_cycle ./gen_star ./gen_cliques ./gen_disconnected ./gen_empty ./gen_random; do \
  $gen 100000 > /tmp/bench_graph.txt && \
  ./invariants_check < /tmp/bench_graph.txt && \
  ./benchmark_runner /tmp/bench_graph.txt; \
done
