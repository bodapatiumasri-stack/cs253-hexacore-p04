### Architect Module (D3) — Block-Cut Forest & Resilience Application

#### 1. Overview & Role Responsibilities
**Role:** Architect (Owner: D3)  
**Primary Objective:** Design the system-wide module interfaces and implement the Block-Cut Forest construction, Lowest Common Ancestor (LCA) preprocessing, $O(\log n)$ vertex-failure connectivity queries, critical node impact ranking, and the end-to-end CLI application.

##### Owned & Integrated Files
* `INTERFACE.md`: System-wide architectural contract defining the shared `Graph` representation, module signatures across D1–D4, and integration/testing rules.
* `graph.hpp`: Core 1-based undirected graph data structure supporting unique 0-based edge IDs and parallel edges.
* `app/block_cut.h` & `app/block_cut.cpp`: Data structures and algorithms for building the Block-Cut Forest, binary lifting tables, and single-vertex failure separation queries.
* `app/ranking.h` & `app/ranking.cpp`: Vulnerability scoring engine that ranks articulation points by the structural fragmentation caused upon removal.
* `app/main.cpp`: Main application driver integrating `Graph`, `StructureA`, `BlockCutForest`, and `ranking` to generate fault-tolerance reports and answer interactive queries.

---

#### 2. System Architecture & Data Pipeline

The Architect module serves as the central orchestration layer connecting low-level graph decomposition with high-level fault-tolerance analytics:

1. **Graph Ingestion (`graph.hpp`)**: Reads $n$ vertices (`1..n`) and $m$ undirected edges (`0..m-1`), retaining both adjacency lists (`adj`) and the original edge list (`edges`) for $O(1)$ edge ID lookups.
2. **Decomposition Ingestion (`StructureAResult`)**: Consumes `articulationPoints`, `bridges`, and edge-grouped `biconnectedComponents` produced by Structure A (D1).
3. **Block-Cut Forest Construction (`buildBlockCutForest`)**: Transforms the graph decomposition into a bipartite forest $F = (V_B \cup V_A, E_F)$, where block nodes ($V_B$) represent biconnected components (or isolated vertices) and articulation nodes ($V_A$) represent cut vertices.
4. **Binary Lifting Preprocessing**: Executes an iterative DFS across all connected components of $F$ to populate `depth`, `component` IDs, and the $2^j$-ancestor jump table `up[LOG][N]`.
5. **Analytics & Interactive Querying**: Computes critical node impact rankings (`rankCriticalNodes`) and answers vertex-failure connectivity queries (`isDisconnectedAfterFailure`).

---

#### 3. Core Algorithms & Implementation Details

##### A. Block-Cut Forest Construction (`app/block_cut.cpp`)
* **Bipartite Node Indexing**:
  * Block nodes are assigned indices `0` through `blockCount - 1`.
  * Articulation nodes are offset and assigned indices `blockCount` through `blockCount + articulationCount - 1`, giving `totalNodes = blockCount + articulationCount`.
* **Isolated Vertex Handling**: Any vertex $v \in [1, n]$ not present in any edge-based biconnected component is assigned its own singleton block node, ensuring complete forest coverage for disconnected graphs.
* **Vertex-to-Forest Mapping (`vertexToNode`)**:
  * Articulation vertices map directly to their dedicated articulation node (`articulationNode[v]`).
  * Non-articulation vertices belong to exactly one biconnected component and map to that component's block node (`vertexBlocks[v][0]`).
* **Edge Construction**: Undirected edges are added between each articulation node `artNode` and every block node `blockId` containing that articulation vertex.

##### B. Iterative Binary Lifting & Tree Distance
* **Stack-Safe Traversal**: Uses an explicit `std::stack<int>` for DFS traversal over the forest rather than recursion, preventing call-stack overflow on deep, path-like graphs.
* **LCA & Shortest Path Distance**:
  * `findLCA(forest, u, v)` equalizes node depths and performs binary jumps in $O(\log N)$ time, returning `-1` if `component[u] != component[v]`.
  * `getDistance(forest, u, v)` computes exact tree distance via:
    $$\text{dist}(u, v) = \text{depth}[u] + \text{depth}[v] - 2 \cdot \text{depth}[\text{LCA}(u, v)]$$

##### C. Vertex-Failure Connectivity Queries (`isDisconnectedAfterFailure`)
Determines whether removing `failedVertex` disconnects surviving vertices `x` and `y`:
1. **Boundary & Endpoint Removal**: Returns `true` if any input vertex is outside `[1, n]` or if `failedVertex == x` or `failedVertex == y`.
2. **Identity & Initial Component Check**: Returns `false` if `x == y` (and survives), or `true` if `nodeX` and `nodeY` already reside in different connected components (`component[nodeX] != component[nodeY]`).
3. **Non-Articulation Fast Path**: If `!isArticulation[failedVertex]`, removing the vertex cannot disconnect any surviving pair, immediately returning `false` in $O(1)$ time.
4. **Tree-Path Separation Test**: Because $F$ is a forest, removing articulation node $f$ separates `nodeX` and `nodeY` if and only if $f$ lies on the unique simple path between them:
   $$\text{dist}(\text{nodeX}, \text{nodeY}) == \text{dist}(\text{nodeX}, f) + \text{dist}(f, \text{nodeY})$$

##### D. Critical Node Impact Ranking (`app/ranking.cpp`)
Quantifies the topological fragmentation caused by removing each articulation vertex $v$:
* **Component Fragmentation Formula**: In a bipartite block-cut forest, the degree of an articulation node (`branches = forest.adj[node].size()`) equals the number of biconnected blocks joined at $v$. Removing $v$ splits its parent connected component into `branches` separate components:
  $$\text{componentsAfterRemoval} = \text{originalComponents} - 1 + \text{branches}$$
  $$\text{impactScore} = \text{componentsAfterRemoval} - \text{originalComponents} = \text{branches} - 1$$
* **Deterministic Ordering**: Articulation points are sorted in descending order of `impactScore` (highest structural impact first), with ties broken by ascending vertex ID (`a.vertex < b.vertex`).

---

#### 4. Complexity Analysis

| Function / Operation | Time Complexity | Space Complexity | Implementation Notes |
| :--- | :--- | :--- | :--- |
| `buildBlockCutForest` | $O((n + m) \log n)$ | $O(n \log n)$ | Deduplicates block vertices via `std::sort`; binary lifting table `up` uses $O(N \log N)$ space where $N \le 2n$. |
| `findLCA` / `getDistance` | $O(\log n)$ | $O(1)$ | Uses precomputed `LOG` jump pointers (`LOG` $\approx \lceil \log_2 N \rceil$). |
| `isDisconnectedAfterFailure` | $O(\log n)$ | $O(1)$ | $O(1)$ fast-paths for non-articulation points; 3 LCA calls for articulation points. |
| `rankCriticalNodes` | $O(n + k \log k)$ | $O(k)$ | Where $k \le n$ is the number of articulation points. |

---

#### 5. Architectural Highlights & Compliance
* **Strict Decoupling (`INTERFACE.md`)**: Separates graph decomposition (D1), independent DSU bridge detection (D2), BFS ground-truth verification (D4), and block-cut query processing (D3), enabling independent testing and clean integration.
* **Disconnected Graph Support**: Explicitly tracks `component[u]` across all forest roots, handling multi-component graphs natively without dummy super-roots.
* **Long-Path Resilience**: Iterative DFS in forest preprocessing satisfies the system testing rule against stack overflow on path-graph edge cases.

### Structure A — Iterative Tarjan Decomposition (`src/structA/`)

#### 1. Key Design Decisions
* **Unified Single-Pass Traversal:** Structure A computes articulation points, bridges, and edge-partitioned biconnected components in a single DFS traversal using discovery timestamps (`disc`), low-link values (`low`), and an auxiliary `edge_stack`.
* **Iterative DFS (`DFSFrame`):** To eliminate recursion depth limits on deep topologies (such as $10^5$-vertex path graphs), recursion is replaced with an explicit heap-allocated `std::vector<DFSFrame>` state stack storing `(u, parent_edge_id, neighbor_idx)`.
* **Multigraph & Edge-Case Handling:**
  * **Parallel Edges:** Parent tree edges are matched by unique `edge_id` (`edge_id == p_edge`) rather than endpoint vertices, ensuring parallel edges act as valid back-edges that lower `low[u]` and form 2-edge BCCs.
  * **Self-Loops:** Filtered immediately (`v == u`) so `edge_stack` and `low[u]` remain uncorrupted.
  * **Isolated Vertices:** Zero-degree vertices explicitly emit an empty edge set (`{}`) into `biconnectedComponents` to maintain complete vertex coverage.
* **Deterministic Sorting:** Output vectors (`articulationPoints`, `bridges`, and each BCC edge list) are sorted with `std::sort` to guarantee deterministic comparison against `Structure B` and `Oracle`.

#### 2. Predicted-Cost Model
* **Time Complexity:**
  * **Traversal & Stack Operations:** $O(V + E)$ — each vertex enters and exits `call_stack` once, and each non-loop edge is traversed twice and pushed/popped from `edge_stack` at most once.
  * **Output Sorting:** $O(V \log V + E \log E)$ worst-case across articulation points, bridges, and BCC edge lists.
  * **Overall:** $O(V + E \log E)$ worst-case ($O(V + E)$ linear traversal-dominated in practice).
* **Space Complexity:** $O(V + E)$ auxiliary memory (`disc`, `low`, `is_ap`, and `call_stack` require $O(V)$; `edge_stack` and stored BCC edge partitions require $O(E)$).

#### 3. Empirical Measurements & Adversarial Verification
* **Scaling vs. Prediction:** Measured runtimes scale linearly with $V + E$ across path, cycle, star, and random multigraph inputs up to $N = 10^5$, closely matching the $O(V + E)$ predicted curve and matching `Structure B` within a small constant factor while avoiding the $O((V + E)^2)$ blowup of the brute-force Oracle.
* **Adversarial Suite Results:** Passed 100% of differential checks against `Oracle` and `Structure B` across all adversarial graph classes (`path`, `cycle`, `xstar`, `cliques`, `random`, `disconnected`, and `empty`), confirming exact agreement on articulation points, bridges, and BCC edge partitions[cite: 1].
## 5. Structure B

**Purpose.** Structure B finds all bridges of the graph by a different method from Structure A. It builds a DFS tree, then uses path marking with union-find to mark every tree edge that lies on a cycle. The unmarked tree edges are the bridges. It also lists the 2-edge-connected components (the pieces left after removing the bridges). The proof of correctness and of the running time is in my derivation D2 (`docs/derivations/D2_250041009.pdf`).

### 5.1 Representation

The input is the common graph from INTERFACE.md: vertices `1..n` and `m` edges, each with an id `0..m-1`. Parallel edges are allowed. The adjacency list stores `(to, id)` pairs, so the edge id travels with the edge.

| Array | Size | Meaning |
|---|---|---|
| `parent[v]`, `parentEdge[v]` | n+1 | parent of v in the DFS tree, and the id of the tree edge to it (-1 for a root) |
| `depth[v]` | n+1 | depth of v in its DFS tree |
| `tin[v]`, `tout[v]` | n+1 | DFS entry and exit times, used to test "is u an ancestor of v" |
| `backEdges` | at most m | pairs (descendant, ancestor) for all non-tree edges |
| `dsu[v]`, `rnk[v]`, `top[v]` | n+1 | union-find parent, rank, and the highest vertex of the set |
| `marked[id]` | m | true if the tree edge `id` lies on a cycle |
| `isBridge[id]` | m | true if edge `id` is a bridge |

Total space is O(V+E).

### 5.2 Operations and their cost

| Operation (function) | What it does | Time |
|---|---|---|
| `buildDFS` | iterative DFS from every unvisited vertex; fills `parent`, `parentEdge`, `depth`, `tin`, `tout` | O(V+E) |
| `findBackEdges` | one pass over all edges; each non-tree edge is stored as (descendant, ancestor) using the `tin`/`tout` test | O(E) |
| `markCycleEdges` (uses `findSet`, `unionSets`) | path marking with union-find, see derivation D2 | O((V+E) α(V)) |
| `findBridges` | collects tree edges that are not marked and sorts their ids | O(V + B log B), B = number of bridges |
| `findTwoEdgeConnectedComponents` | BFS that does not cross bridges; sorts each component and the list of components | O(V+E) + O(V log V) for sorting |
| `computeStructureB` (whole) | calls the above in order | O((V+E) α(V) + V log V) |

The two sorts only fix the output order so that it can be compared with Structure A and the oracle. The core of Structure B (finding the bridges) is O((V+E) α(V)). 
### 5.3 Invariants it maintains

1. **No cross edges.** Every non-tree edge joins a vertex to one of its ancestors (D2, Lemma 1). `findBackEdges` relies on this, because it only tests the two ancestor directions.
2. **DSU sets are connected pieces.** Every union-find set is a connected part of the DFS tree, and `top` of the set is its highest vertex (smallest depth).
3. **Marked means merged.** A tree edge is marked exactly when its two endpoints are in the same set.
4. **Marked means on a cycle.** After `markCycleEdges`, a tree edge is marked exactly when some back edge covers it, which is the same as lying on a cycle.
5. **Output.** `bridges` is sorted without duplicates, and every bridge is a tree edge. The components partition the vertex set, and no non-bridge edge joins two different components.


### 5.4 Design decisions and rejected alternatives

- **Iterative DFS instead of recursive.** A recursive DFS overflows the call stack on a path of 100,000 vertices, which is one of the adversarial inputs. I keep my own stack and an iterator index `it[u]` per vertex. The cost is slightly longer code.
- **Skip the parent by edge id, not by parent vertex.** Skipping "the vertex I came from" would ignore a second parallel edge to the parent, and that parallel edge makes the tree edge lie on a cycle. Skipping by id handles this correctly.
- **Ancestor test with `tin`/`tout`.** I store entry and exit times and test ancestry in O(1), instead of recording back edges during the DFS or using LCA. This keeps the DFS simple, and the back edges come from one separate pass.
- **Path marking with union-find instead of walking up for every back edge.** The simple method walks from the descendant up to the ancestor for each back edge. By the following argument it can take O(V·E): in a path of V vertices with E back edges that all go to the root, every walk costs O(V). With union-find, every later walk jumps over the already-marked part in one `Find`, so each tree edge is marked only once.
- **Union by rank plus a `top[]` array, not path compression alone.** Path compression alone is simpler, but its standard bound is O(log V) amortised per operation, which does not match the α(V) in the statement. With rank, the representative of a set is no longer always its highest vertex, so `top[]` stores that separately. The cost is extra memory and extra `Find` calls per round. [Write what you measured, for example "the rank version was X% slower at V = 10^5 because of this constant factor" or "no visible difference". Write only what you measured.]
- **A different method from Structure A.** Structure A uses Tarjan's low-link values. I did not use low-link in B, so that the two structures can check each other.
- **2-edge-connected components by BFS avoiding bridges.** This is simple and O(V+E). The alternative, finding components with a stack inside the DFS, mixes component finding into the bridge code and makes the cross-check against A less independent.

### 5.5 Links

Derivation: D2 (`docs/derivations/D2_250041009.pdf`). Code: `structB.cpp`. Main pull requests:
pull requests-
https://github.com/bodapatiumasri-stack/cs253-hexacore-p04/pull/7
https://github.com/bodapatiumasri-stack/cs253-hexacore-p04/pull/14
https://github.com/bodapatiumasri-stack/cs253-hexacore-p04/pull/38



## Structure D — Oracle Module

### 1. Objective

The Oracle module verifies the correctness of Structure A and Structure B using a brute-force BFS approach. It identifies articulation points and bridges by checking the number of connected components after removing individual vertices and edges.

### 2. Working and Main Functions

The Oracle first calculates the original number of connected components. It then removes each vertex and edge individually and performs BFS to check whether the component count increases.

| Function | Description |
|---|---|
| `countComponents()` | Counts connected components using BFS while ignoring a selected vertex or edge. |
| `oracleArticulationPoints()` | Identifies vertices whose removal increases the component count. |
| `oracleBridges()` | Identifies edges whose removal increases the component count. |
| `runOracle()` | Performs all checks and stores the results. |
| `compareResults()` | Compares Oracle results with Structure A and Structure B and reports MATCH or MISMATCH. |

### 3. Complexity Analysis

| Function | Time Complexity |
|---|---|
| `countComponents()` | O(V + E) |
| `oracleArticulationPoints()` | O(V(V + E)) |
| `oracleBridges()` | O(E(V + E)) |
| `runOracle()` | O((V + E)^2) |

Auxiliary Space Complexity: O(V + E).

### 4. Testing

The Oracle was tested using different graph structures to verify its correctness.

| Test Case | Purpose |
|---|---|
| Path | Tests articulation points and bridges. |
| Cycle | Tests a connected cycle. |
| Star | Tests the central articulation point. |
| Disconnected graph | Tests multiple components. |
| Complete graph | Tests high connectivity. |
| Isolated vertices | Tests vertices without edges. |
| Parallel edges | Tests multiple edges between vertices. |
| Self-loops | Tests self-loop handling. |

### 5. Limitations

Since the Oracle uses repeated BFS, its execution becomes expensive for large graphs. Therefore, `oracleTooLarge()` skips execution when the number of vertices exceeds 5,000 or the number of edges exceeds 25,000.

The Oracle is mainly intended for verifying smaller test graphs, while the efficient algorithms handle larger inputs.

### 6. Conclusion

The Oracle provides an independent correctness-checking mechanism for the project. By comparing its BFS-based results with Structure A and Structure B, it helps detect errors and improves the reliability of the overall implementation.

## 8. Adversarial Inputs

All generators are located in `adversary/`. Each generator prints a graph in the common input format (`n m`, followed by one `u v` pair per line). Vertices are numbered from 1 to n, and the generated graphs contain no self-loops or duplicate edges. This allows Structure A, Structure B, and the Oracle to process the same input. Large graphs are generated when needed and are not committed to the repository.

### 8.1 Generators and Targets

| Generator | How it builds the graph | Target | Expected answer |
|---|---|---|---|
| `path n` | Connects consecutive vertices with edges `(i, i+1)` | Root rule, bug A (root has one child); deep DFS | n−2 articulation points, n−1 bridges; vertex 1 is not an articulation point |
| `cycle n` | Builds a path and adds edge `(n, 1)` to close the cycle | Long simple cycle; root rule, bug A | 0 articulation points, 0 bridges, 1 biconnected block |
| `star n` | Connects vertex 1 to every other vertex | Root rule, bug B (root has n−1 children) | 1 articulation point (vertex 1), n−1 bridges |
| `cliques k s` | Builds k cliques of size s sharing vertex 1 (k ≥ 2, s ≥ 3) | Cliques joined at one vertex | 1 articulation point (vertex 1), 0 bridges, k biconnected blocks |
| `disconnected n` | Creates a path, a cycle, a star, and one isolated vertex (n ≥ 10) | DFS restarts; root rule in each DFS tree; isolated vertex | Let a = ⌊(n−1)/3⌋ and c = n−1−2a. The graph has a−1 articulation points and a+c−2 bridges |
| `empty n` | Creates n vertices and no edges | Isolated vertices | 0 articulation points, 0 bridges |
| `random n m seed` | Generates seeded random edges and shuffles vertex labels | General inputs | Results are compared with the Oracle |

For large-input generation, `cliques 100 142` creates 14,101 vertices and 1,001,100 edges. `path 1000000` creates one million vertices, and `random 100000 1000000 seed` creates 100,000 vertices and one million edges. These large inputs were not included in the combined testing described below.

### 8.2 Worst-Case Argument (From D5)

An undirected DFS has no cross edges. Every non-tree edge connects a vertex to one of its ancestors, which is why low-link values can capture the back edges needed to identify articulation points and bridges.

The DFS root needs a separate rule because it has no parent. It is an articulation point only if it has at least two DFS-tree children.

A program that incorrectly applies the normal non-root test to the root may report a false articulation point on `path` and `cycle` (bug A). A program that skips the root entirely may fail to identify the articulation point in `star` (bug B).

The `disconnected` generator checks whether the root rule is applied separately to every DFS tree. Its star centre is the root of its own component and is not vertex 1.

A long path can also make recursive DFS reach a very large depth, potentially causing stack overflow. An iterative DFS avoids relying on deep recursive calls.

The complete proof is provided in `docs/derivations/D5_250041015.pdf`.

### 8.3 What Was Measured

- All seven adversarial generators were compiled successfully and tested on small inputs. Their generated graphs were checked against the expected graph properties.
- **Combined run on `main` (16 Nov 2026):** I compiled the local checker (`adversary_check.cpp`) together with the team's Structure A, Structure B, and Oracle using `g++ -std=c++17 -Wall -Wextra -I. -DUSE_TEAM_ORACLE` on my friend's laptop. The only compiler warning shown was an unused `argv` parameter in the local checker.
- I compiled all seven generators separately and ran them through the checker. All eight test cases returned `RESULT: ALL OK`: `path 5`, `path 10`, `cycle 10`, `star 10`, `empty 5`, `disconnected 10`, `cliques 3 4`, and `random 50 80 1`.
- For `path 5`, Structure A reported articulation points `{2, 3, 4}` and bridges `{0, 1, 2, 3}`. Structure B reported the same bridge IDs.
- The random generator test used seed 1 in `random 50 80 1`.
- Only small inputs were tested in the combined run. No full-scale testing is claimed here.

### 8.4 Limitations

- The combined run covers small inputs only, so no full-scale result (10⁵ vertices or 10⁶ edges) is claimed here.
- The `random` generator accepts at most m = n(n−1)/4 edges to keep generation manageable.
- There is no generator for self-loops or parallel edges because they are outside the agreed input format.
- I did not take timing measurements. Performance measurements are covered by the Analyst.

## 9. Analyst — Predicted Cost Model & Empirical Benchmarking

**Role:** Analyst (Owner: D6 — Sahasra, `250041023`)  
**Derivation & Reports:** `docs/derivations/D6_250041023.pdf`, `docs/theoritical_cost_models.md`, `docs/final_analyst_report.md`  
**Benchmark & Plotting Code:** `benchmark/benchmark_runner.cpp`, `benchmark/plot_results.py`, `results/results.csv`

---

### 9.1 Pre-Measurement Theoretical Cost Model (D6)

Before running empirical benchmarks, asymptotic time and space bounds were established for all three decomposition engines:

| Engine / Module | Worst-Case Time | Auxiliary Space | Theoretical Justification |
| :--- | :--- | :--- | :--- |
| **Structure A** (Iterative Tarjan DFS) | $\mathcal{O}(V + E)$ | $\mathcal{O}(V + E)$ | Single-pass iterative DFS visits each vertex once and traverses each undirected edge twice, updating $\text{disc}[u]$ and $\text{low}[u]$ in $\mathcal{O}(1)$ time per edge using explicit call and edge stacks. |
| **Structure B** (Union-Find Path Marking) | $\mathcal{O}((V + E)\,\alpha(V))$ | $\mathcal{O}(V + E)$ | Builds a DFS spanning tree in $\mathcal{O}(V + E)$ time, then contracts cycle tree-edges along back-edges using path-compressed, rank-ordered Disjoint-Set Union ($\alpha(V) \le 4$ for all practical $V$). |
| **Oracle Baseline** (Brute-Force BFS) | $\mathcal{O}(V(V + E))$ | $\mathcal{O}(V + E)$ | Masks each vertex/edge individually and executes a full $\mathcal{O}(V + E)$ BFS component count across all $V$ vertices, yielding quadratic scaling. |

**Pre-Measurement Performance Predictions:**
1. **Validation Regime ($V \le 5,000$):** All three engines complete within practical bounds, enabling ground-truth Oracle verification.
2. **Scalability Regime ($V > 5,000$ to $V = 100,000$):** The Oracle diverges quadratically $\mathcal{O}(V^2 + VE)$ and is bypassed via `oracleTooLarge(g)`, while Structures A and B maintain linear growth.
3. **Structure A vs. Structure B Constant-Factor Separation:** Structure B is predicted to exhibit higher constant-factor overhead than Structure A due to multi-phase processing (DFS tree construction, back-edge extraction, DSU cycle marking, and BFS component collection) and non-contiguous pointer chasing during DSU path compression.

---

### 9.2 Benchmark Harness & Experimental Setup

* **Benchmark Harness (`benchmark/benchmark_runner.cpp`):**
  * Loads graphs in standard `n m` edge-list format into the shared 1-based `Graph` representation (`graph.hpp`) with unique 0-based edge IDs.
  * Measures wall-clock execution time independently for `analyzeStructureA`, `computeStructureB`, and `runOracle` using `std::chrono::high_resolution_clock` (reported in milliseconds).
  * Automatically sorts output vectors (`articulationPoints`, `bridges`) and asserts exact equality across Structure A, Structure B, and the Oracle (`PASS` / `MISMATCH`) whenever `!oracleTooLarge(g)`, logging all metrics to `results.csv`.
* **Test Suite Coverage:** **35 adversarial datasets** spanning **7 topologies** (`cliques`, `cycle`, `disconnected`, `empty`, `path`, `random`, `star`) across **5 vertex scales** ($V \approx 1,000$, $5,000$, $10,000$, $50,000$, and $100,000$, up to $E = 2,450,000$).
* **Compilation & Execution Environment:** Windows 11 (ARM64), compiled with `g++.exe (MinGW.org GCC-6.3.0-1) 6.3.0` (`-std=c++11 -O2`), Processor: `Snapdragon(R) X - X126100 - Qualcomm(R) Oryon(TM) CPU`.

---

### 9.3 Empirical Results & Verification Summary

* **100% Ground-Truth Verification:** On all **13** Oracle-eligible validation graphs ($V \le 5,000$), Structure A and Structure B achieved **13 / 13 `PASS`** agreement with the Oracle; the remaining **22** large-scale datasets ($V \ge 10,000$ or $E > 50,000$) were marked `SKIPPED` by `oracleTooLarge(g)`.
* **Validation Regime ($V \approx 1,000$ and $5,000$):**

| Dataset | $V$ | $E$ | Structure A (ms) | Structure B (ms) | Oracle (ms) | Check |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: |
| `cliques_1000.txt` | 981 | 24,500 | 0.000 | 13.591 | 2,524.930 | **PASS** |
| `cycle_1000.txt` | 1,000 | 1,000 | 0.842 | 1.342 | 31.539 | **PASS** |
| `cycle_5000.txt` | 5,000 | 5,000 | 0.000 | 5.769 | 862.235 | **PASS** |
| `disconnected_1000.txt` | 1,000 | 997 | 1.505 | 4.008 | 29.697 | **PASS** |
| `disconnected_5000.txt` | 5,000 | 4,997 | 3.471 | 0.000 | 899.348 | **PASS** |
| `empty_1000.txt` | 1,000 | 0 | 0.000 | 0.000 | 1,171.380 | **PASS** |
| `empty_5000.txt` | 5,000 | 0 | 5.451 | 0.000 | 21,746.500 | **PASS** |
| `path_1000.txt` | 1,000 | 999 | 2.004 | 4.957 | 62.608 | **PASS** |
| `path_5000.txt` | 5,000 | 4,999 | 3.953 | 8.096 | 966.136 | **PASS** |
| `random_1000.txt` | 1,000 | 3,000 | 3.809 | 0.000 | 100.394 | **PASS** |
| `random_5000.txt` | 5,000 | 15,000 | 11.257 | 4.008 | 4,435.290 | **PASS** |
| `star_1000.txt` | 1,000 | 999 | 0.000 | 8.771 | 24.213 | **PASS** |
| `star_5000.txt` | 5,000 | 4,999 | 0.000 | 11.624 | 835.784 | **PASS** |

*(Note: `0.000 ms` values indicate sub-millisecond execution below the OS steady-clock timer resolution).*

* **Extreme-Scale Regime ($V \approx 100,000$ Across All 7 Topologies):**

| Dataset ($V \approx 100,000$) | $V$ | $E$ | Structure A (ms) | Structure B (ms) | Speedup (A vs. B) |
| :--- | :---: | :---: | :---: | :---: | :---: |
| `cliques_100000.txt` | 98,001 | 2,450,000 | 125.024 | 132.704 | $1.06\times$ faster |
| `cycle_100000.txt` | 100,000 | 100,000 | 13.426 | 33.894 | $2.52\times$ faster |
| `disconnected_100000.txt` | 100,000 | 99,997 | 46.688 | 103.839 | $2.22\times$ faster |
| `empty_100000.txt` | 100,000 | 0 | 29.007 | 153.933 | $5.31\times$ faster |
| `path_100000.txt` | 100,000 | 99,999 | 32.917 | 156.573 | $4.76\times$ faster |
| `random_100000.txt` | 100,000 | 300,000 | 67.515 | 121.513 | $1.80\times$ faster |
| `star_100000.txt` | 100,000 | 99,999 | 46.910 | 159.229 | $3.39\times$ faster |

---

### 9.4 Predicted vs. Measured Cost Synthesis & Figures

* **Quadratic Oracle Blowup Confirmed:** Scaling $V$ by $5\times$ ($1,000 \to 5,000$) increased Oracle runtime from **1,171.380 ms** to **21,746.500 ms** ($18.56\times$) on `empty` graphs and from **100.394 ms** to **4,435.290 ms** ($44.18\times$) on `random` graphs, validating the $\mathcal{O}(V(V+E))$ model and the `oracleTooLarge` cutoff.
* **Linear Scaling & Single-Pass Advantage Confirmed:** Both Structure A and Structure B scaled linearly across a $100\times$ increase in $V$ ($1,000 \to 100,000$) without recursion stack overflows. At $V \approx 100,000$, Structure A outperformed Structure B across **all 7 topologies** (mean **51.64 ms** vs. **123.10 ms**), confirming Prediction 3 that single-pass contiguous array access avoids Structure B's multi-pass DSU overhead.
* **Generated Benchmark Figures (`benchmark/plot_results.py` → `docs/plots/`):**
  1. `docs/plots/oracle_vs_structures_log.png` — Log-scale runtime comparison validating Oracle quadratic divergence against linear Structures A and B ($V = 1,000$ to $5,000$).
  2. `docs/plots/scalability_comparison.png` — Mean execution time scaling across all 7 topologies from $V = 1,000$ to $V = 100,000$.
  3. `docs/plots/topology_100k_comparison.png` — Grouped bar comparison across all 7 adversarial topologies at $V \approx 100,000$.

