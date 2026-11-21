# Project 04: Critical nodes and links in a network
Team hexacore, CS 253 / MA 253, Autumn 2026

## Team and roles

| Name | Roll no. | GitHub | Role | Module folder | Derivation |
|------|----------|--------|------|---------------|------------|
| Rishika Goyal | 250008029 | @che250008029-netizen | Structure A owner | src/structA/ | D1 |
| Bodapati Uma Sri | 250041009 | @bodapatiumasri-stack | Structure B owner | src/structB/ | D2 |
| Deepali Chauhan | 250041014 | @DeepaliChauhan07 | Architect | app/ | D3 |
| Keetha Akshaya | 250041021 | @akshaya129 | Oracle owner | oracle/ | D4 |
| Dharavath Mamatha | 250041015 | @mamathadharavath35-a11y | Adversary | adversary/ | D5 |
| Kunreddy Sahasra Reddy | 250041023 | @sahasrakunreddy | Analyst | benchmark/, docs/ | D6 |

## Summary
The project finds the critical vertices (articulation points), the critical links (bridges) and the biconnected components of an undirected network of up to 10^5 vertices and 10^6 edges, and it answers whether the failure of one vertex separates two others. Structure A is an iterative Tarjan low-link DFS. Structure B finds bridges independently with a DFS tree and union-find path marking. A brute-force oracle checks both on graphs of up to 5,000 vertices, and an application builds a block-cut forest for the failure queries. Both structures scale linearly up to 100,000 vertices and 2,450,000 edges; Structure A is about 2.4 times faster than B on average (51.64 ms against 123.10 ms at 100,000 vertices).

## Requirements

| Member / purpose | OS | Compiler and flags |
|---|---|---|
| Analyst (benchmarks) | Windows 11 (ARM64), Snapdragon X | g++ (MinGW.org GCC-6.3.0-1) 6.3.0, `-std=c++11 -O2` |
| Oracle owner | Windows 11 version 26H2 (OS Build 26300.9818), x86_64 | g++ 16.2.0 (MinGW-W64 x86_64-ucrt-posix-seh), `-std=c++17` |
| Structure B owner | Windows 11 Home version 25H2 (OS Build 26200.9974), x86_64 (AMD64) | g++ (MinGW.org GCC-6.3.0-1) 6.3.0, `-std=c++17 -O2` |
| Architect | Windows 11 (64-bit, x86_64) | g++ (MinGW-w64 GCC), `-std=c++17 -O2` |
| Adversary | Windows 11 Home Single Language, version 25H2 (OS Build 26200.9974) | g++ (MinGW.org GCC-6.3.0-1) 6.3.0, target `mingw32`; the generators in `adversary/` compiled with `g++ -O2` (default standard of GCC 6.3) |
| Structure A owner | Windows 11 (64-bit, x86_64) | g++ (MinGW-w64 GCC), `-std=c++17 -O2` |

`scripts/run_all.sh` uses `-std=c++14`. If a file needs C++17, run `STD=-std=c++17 bash scripts/run_all.sh`. Python 3 with matplotlib is needed only for the figures. No external libraries are used; library containers appear only in tests and data generation.

## Build
    bash scripts/run_all.sh build
This writes the programs `integration_test`, `invariants_check`, `app_demo` and `benchmark_runner` into `build/`.

## Quick start
    g++ -O2 adversary/star.cpp -o gen_star
    ./gen_star 5
Expected output (a star with centre 1):

    5 4
    1 2
    1 3
    1 4
    1 5

Application example (a graph, then the number of queries, then queries `x y v` testing whether removing `v` disconnects `x` and `y`):

    printf "5 5\n1 2\n2 3\n3 1\n3 4\n4 5\n3\n1 5 3\n1 5 4\n1 2 3\n" | ./build/app_demo

Expected output:

    --- NETWORK VULNERABILITY REPORT ---
    Total Nodes: 5 | Total Edges: 5
    Articulation Points (2): 3 4
## Tests
    ./build/integration_test
Seed 12345. A passing run ends with: ALL TESTS PASSED (failures: 0). It compares Structure A, Structure B and the oracle on 8 named graphs and 500 random graphs, and runs A and B on a path of 200,000 vertices.

    ./build/invariants_check q < graph.txt
The invariant checker reads one graph and ends with ALL INVARIANTS OK or INVARIANT VIOLATED.

Example outputs of the oracle comparison:

### Two triangles joined by a path
    Articulation Points: 3 4 5
    Bridges (Edge IDs):  3 4
    Structure A - Articulation Points: MATCH
    Structure A - Bridges: MATCH
    Structure B - Bridges: MATCH

### Parallel edges
    Articulation Points: 2
    Bridges (Edge IDs):  2
    Structure A - Articulation Points: MATCH
    Structure A - Bridges: MATCH
    Structure B - Bridges: MATCH

## Benchmarks and figures
    bash scripts/run_all.sh
This builds everything, runs the tests and the application, creates the 35 benchmark graphs in datasets/ with the generators in adversary/ (random seed 12345, see scripts/gen_datasets.sh), runs the benchmark on each (results/results.csv) and regenerates the three figures in docs/plots/. [State roughly how long it takes.] The graphs are not committed; the script recreates them.

## Repository map
- adversary/: seven graph generators (path, cycle, star, cliques, disconnected, empty, random)
- app/: block-cut forest, ranking and the command-line application
- benchmark/: benchmark runner and plotting script
- docs/: derivations (docs/derivations/), plots, slides, cost model
- oracle/: brute-force oracle, differential test, invariant checkers
- results/: benchmark results (results.csv)
- scripts/: run_all.sh (build, tests, application, benchmarks, figures) and gen_datasets.sh
- src/: Structure A (structA/) and Structure B (structB/)
- graph.hpp, INTERFACE.md, REPORT.md, TOOLS.md

## Results at a glance

| Check | Result |
|---|---|
| Oracle comparison (V up to 5,000) | 13 of 13 pass |
| Random differential test (seed 12345) | 500 of 500 match |
| Bridge agreement of A and B on the 35 benchmark graphs | 35 of 35 |
| Mean time at 100,000 vertices | Structure A 51.64 ms, Structure B 123.10 ms |

Full analysis: REPORT.md, sections 7 and 9.

## Known limitations
- The oracle runs only up to 5,000 vertices and 25,000 edges (the 25,000 is a team choice); larger graphs are checked by comparing Structure A with Structure B.
- The differential test uses 500 random graphs and 8 named graphs, not 10^6 seeded operations, and it does not shrink failing cases.
- The adversarial run through A, B and the oracle used small inputs.
- [State the hidden-test interface status and any folder names that differ from the required layout.]

See REPORT.md, section 12, for the full list.

## Tools and references
AI and other tools are declared in TOOLS.md. The papers and books are listed in REPORT.md, section 13 (Tarjan 1972, Hopcroft and Tarjan 1973, Tarjan 1974, and others).
