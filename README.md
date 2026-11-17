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
```

## One Command to Run Tests, Application, and Every Benchmark
Run the differential integration tests, structural invariant checks, interactive Block-Cut Forest application, and full-scale ($V = 100{,}000$) benchmarks across every adversarial topology in a single command:

```bash
./integration_test && \
printf "5 5\n1 2\n2 3\n3 1\n3 4\n4 5\n3\n1 5 3\n1 5 4\n1 2 3\n" | ./app_demo && \
for gen in ./gen_path ./gen_cycle ./gen_star ./gen_cliques ./gen_disconnected ./gen_empty ./gen_random; do \
  $gen 100000 > /tmp/bench_graph.txt && \
  ./invariants_check < /tmp/bench_graph.txt && \
  ./benchmark_runner /tmp/bench_graph.txt; \
done
```
## Benchmarks and figures

Before rerunning the full benchmark suite, remove or clear any previous `results/results.csv` so appended rows are not duplicated:

```bash
# 1. Compile the benchmark and cross-verification harness
g++ -std=c++11 -O2 benchmark/benchmark_runner.cpp src/structA/*.cpp src/structB/*.cpp oracle/*.cpp -o benchmark/runner

# 2. Run all 35 adversarial datasets (writes results/results.csv)
rm -f results/results.csv
for f in adversary/*.txt; do
    ./benchmark/runner "$f"
done

# 3. Regenerate all 3 performance figures (writes docs/plots/*.png)
python benchmark/plot_results.py

## Results at a glance

* **Correctness Verification:** **13 / 13 `PASS`** against the BFS Oracle on all eligible graphs (`V ≤ 5,000` and `E ≤ 25,000`), and **35 / 35** exact bridge-set agreement between Structure A and Structure B across all datasets.
* **Extreme-Scale Performance (`V ≈ 100,000`):** Structure A completed every topology in **13.43 ms – 125.02 ms** (mean **51.64 ms**), while Structure B completed in **33.89 ms – 159.23 ms** (mean **123.10 ms**).

| Dataset (`V ≈ 100,000`) | Vertices (`V`) | Edges (`E`) | Structure A (ms) | Structure B (ms) | Oracle (ms) | Status |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: |
| `cliques_100000.txt` | 98,001 | 2,450,000 | 125.024 | 132.704 | SKIPPED | Verified (A = B) |
| `cycle_100000.txt` | 100,000 | 100,000 | 13.426 | 33.894 | SKIPPED | Verified (A = B) |
| `disconnected_100000.txt` | 100,000 | 99,997 | 46.688 | 103.839 | SKIPPED | Verified (A = B) |
| `empty_100000.txt` | 100,000 | 0 | 29.007 | 153.933 | SKIPPED | Verified (A = B) |
| `path_100000.txt` | 100,000 | 99,999 | 32.917 | 156.573 | SKIPPED | Verified (A = B) |
| `random_100000.txt` | 100,000 | 300,000 | 67.515 | 121.513 | SKIPPED | Verified (A = B) |
| `star_100000.txt` | 100,000 | 99,999 | 46.910 | 159.229 | SKIPPED | Verified (A = B) |

See [`REPORT.md`](./REPORT.md) (Section 9) and [`docs/final_analyst_report.md`](./docs/final_analyst_report.md) for full theoretical derivations, all 35 dataset rows, and scaling plots.

## Known limitations

* **Oracle Scalability Cutoff:** Because the brute-force BFS Oracle runs in O((V + E)²) time, `oracleTooLarge(g)` skips Oracle execution whenever `V > 5,000` or `E > 25,000` (e.g., `cliques_5000.txt` has `E = 122,500` and is skipped). Consequently, 13 of the 35 datasets are Oracle-verified directly, while the 22 large datasets are cross-verified between Structure A and Structure B.
* **Sub-Millisecond Timer Granularity:** `benchmark_runner.cpp` executes a single timed pass per graph using `std::chrono::high_resolution_clock`. Fast runs under ~0.05 ms on small or sparse graphs (such as `cliques_1000` for Structure A or `disconnected_5000` for Structure B) record `0.000 ms` due to single-run clock resolution; multi-iteration averaging would resolve sub-millisecond differences.
* **Structure B Output Scope:** Structure B independently identifies bridges and 2-edge-connected components via DSU cycle marking, whereas articulation points and biconnected components are computed solely by Structure A (and consumed by the Block-Cut Forest).
* **Append-Mode CSV Output:** `benchmark_runner` appends to `results/results.csv` (`std::ios::app`), so `results/results.csv` must be cleared before rerunning the full 35-graph suite to prevent duplicate entries.
