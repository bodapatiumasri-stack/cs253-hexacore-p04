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
