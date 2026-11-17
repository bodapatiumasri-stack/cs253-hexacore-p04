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
## Build & Run (Single-Sequence Verification)

Compile all executables from the repository root (`g++ -O2 -std=c++17`):

```bash
g++ -O2 -std=c++17 -I. src/structA/structA.cpp src/structB/structB.cpp app/block_cut.cpp oracle/oracle.cpp oracle/oracle_integration_test.cpp -o integration_test
g++ -O2 -std=c++17 -I. src/structA/structA.cpp src/structB/structB.cpp oracle/oracle.cpp oracle/invariants_check.cpp -o invariants_check
g++ -O2 -std=c++17 -I. src/structA/structA.cpp src/structB/structB.cpp app/block_cut.cpp app/ranking.cpp app/main.cpp -o app_demo
g++ -O2 -std=c++17 adversary/path.cpp -o gen_path
g++ -O2 -std=c++17 -I. src/structA/structA.cpp src/structB/structB.cpp oracle/oracle.cpp benchmark/benchmark_runner.cpp -o benchmark_runner
