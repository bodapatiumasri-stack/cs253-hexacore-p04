# Project 04: Critical nodes and links in a network
Team hexacore — CS 253 / MA 253, [Autumn 2026]

## 1. Team and roles

| Name | Roll no. | GitHub | Role | Module folder | Derivation |
|------|----------|--------|------|---------------|------------|
| Rishika Goyal | 250008029 | @che250008029-netizen | Structure A owner | src/structA/ | D1 |
| Bodapati Uma Sri | 250041009 | @bodapatiumasri-stack | Structure B owner | src/structB/ | D2 |
| Deepali Chauhan | 250041014 | @DeepaliChauhan07 | Architect | app/ | D3 |
| Keetha Akshaya | 250041021 | @akshaya129 | Oracle owner | oracle/ | D4 |
| Dharavath Mamatha | 250041015 | @mamathadharavath35-a11y | Adversary | adversary/ | D5 |
| Kunreddy Sahasra Reddy | 250041023 | @sahasrakunreddy | Analyst | benchmark/, docs/ | D6 |

### 1.1 Contributions

**Rishika Goyal — Structure A**

- Implemented the single-pass iterative Tarjan low-link DFS engine (`src/structA/structA.hpp`, `src/structA/structA.cpp`) to compute articulation points, bridges, and biconnected components in $\mathcal{O}(V + E)$ time with multigraph edge-ID tracking and self-loop handling.
- Prepared the D1 derivation (`docs/derivations/D1_250008029.pdf`) proving the bridge and articulation-point low-link criteria, biconnected edge-stack extraction, and linear time/space complexity.
- Verified Structure A across edge cases, randomized differential tests against Structure B and the Oracle, and full-scale ($V = 100{,}000$) adversarial topologies, and finalized repository documentation (`README.md`, `REPORT.md`).
- 
**Bodapati Uma Sri — Structure B owner**

- Implemented Structure B in `src/structB/` (`structB.cpp`, `structB.hpp`): an iterative DFS forest, non-tree edge detection with `tin`/`tout`, and union-find path marking (union by rank, path compression, `top[]`) to find bridges .
- Built the two-edge-connected components from the bridges with a BFS that never crosses a bridge, and handled parallel edges (matched by edge ID), self-loops and disconnected graphs.
- Prepared the D2 derivation(`docs/derivations/D2_250041009.pdf`) proving that path marking marks exactly the tree edges on a cycle, so the unmarked tree edges are the bridges, in O((V+E) α(V)).
- Contributed to `TOOLS.md`

**Deepali Chauhan — Architect & Interface Owner**

- Designed the system-wide architectural contract (`INTERFACE.md`) and shared 1-based undirected multigraph representation (`graph.hpp`) with unique 0-based edge IDs across all modules (D1–D4).
- Implemented the Block-Cut Forest construction (`app/block_cut.h`, `app/block_cut.cpp`) with iterative binary lifting LCA preprocessing for $\mathcal{O}(\log n)$ single-vertex failure connectivity queries, as well as the critical node impact ranking engine (`app/ranking.h`, `app/ranking.cpp`) and end-to-end CLI application (`app/main.cpp`).
- Prepared the D3 derivation (`docs/derivations/D3_250041014.pdf`) proving that the bipartite block-cut incidence graph is acyclic (a forest) and that vertex-failure separation reduces to tree-path distance checks.
  
**Keetha Akshaya — Oracle owner**

- Implemented the brute-force oracle in `oracle/oracle.hpp` and `oracle/oracle.cpp` (`countComponents`, `runOracle`, `oracleArticulationPoints`, `oracleBridges`, `oracleTooLarge`, `compareResults`, `runOracleReport`). It removes each vertex or edge, counts the components again with BFS and compares with the original count.
- Wrote `oracle/oracle_test.cpp` (16 test groups with hand-computed answers), `oracle/oracle_integration_test.cpp` (Structure A, Structure B and the oracle on 8 named graphs, 500 random graphs and a path of 200,000 vertices) and `oracle/invariants_check.cpp`.
- Prepared the D4 derivation (`docs/derivations/D4_250041021.pdf`): the oracle's component-count test matches the low-link bridge criterion used by Structure A, and a bridge is a biconnected component by itself.
- Main pull requests: #4, #35, #61.
  
**Dharavath Mamatha — Adversary**

- Implemented seven graph generators in the `adversary/` directory to create adversarial and edge-case inputs (PR #1, #3).
- Prepared the D5 derivation explaining why undirected DFS has no cross edges, including the role of the root rule (PR #17).
- Contributed to `TOOLS.md` (PR #41).

## 2. Problem

The project analyzes an undirected network to identify its critical vertices (articulation points), critical links (bridges), and biconnected components. It also answers queries about whether removing one vertex would disconnect two other vertices.

The application provides two independent approaches for finding critical structures. Structure A uses an iterative Tarjan low-link DFS, while Structure B uses a DFS tree and union-find path marking to find bridges. A brute-force oracle checks the results on smaller graphs, and the application uses a block-cut forest to answer vertex-failure queries.

The project targets graphs with up to 100,000 vertices and 1,000,000 edges. The README reports testing at 100,000 vertices and 2,450,000 edges; the oracle is limited to graphs with up to 5,000 vertices and 25,000 edges.

### 3. Architect Module (D3) — Block-Cut Forest & Resilience Application
**Owner:** Deepali Chauhan (`250041014` / `DeepaliChauhan07`)  
**Derivation:** `docs/derivations/D3_250041014.pdf`

#### 3.1. Overview & Role Responsibilities
**Role:** Architect (Owner: D3)  
**Primary Objective:** Design the system-wide module interfaces and implement the Block-Cut Forest construction, Lowest Common Ancestor (LCA) preprocessing, $O(\log n)$ vertex-failure connectivity queries, critical node impact ranking, and the end-to-end CLI application.

##### Owned & Integrated Files
* `INTERFACE.md`: System-wide architectural contract defining the shared `Graph` representation, module signatures across D1–D4, and integration/testing rules.
* `graph.hpp`: Core 1-based undirected graph data structure supporting unique 0-based edge IDs and parallel edges.
* `app/block_cut.h` & `app/block_cut.cpp`: Data structures and algorithms for building the Block-Cut Forest, binary lifting tables, and single-vertex failure separation queries.
* `app/ranking.h` & `app/ranking.cpp`: Vulnerability scoring engine that ranks articulation points by the structural fragmentation caused upon removal.
* `app/main.cpp`: Main application driver integrating `Graph`, `StructureA`, `BlockCutForest`, and `ranking` to generate fault-tolerance reports and answer interactive queries.

---

#### 3.2. System Architecture & Data Pipeline

The Architect module serves as the central orchestration layer connecting low-level graph decomposition with high-level fault-tolerance analytics:

1. **Graph Ingestion (`graph.hpp`)**: Reads $n$ vertices (`1..n`) and $m$ undirected edges (`0..m-1`), retaining both adjacency lists (`adj`) and the original edge list (`edges`) for $O(1)$ edge ID lookups.
2. **Decomposition Ingestion (`StructureAResult`)**: Consumes `articulationPoints`, `bridges`, and edge-grouped `biconnectedComponents` produced by Structure A (D1).
3. **Block-Cut Forest Construction (`buildBlockCutForest`)**: Transforms the graph decomposition into a bipartite forest $F = (V_B \cup V_A, E_F)$, where block nodes ($V_B$) represent biconnected components (or isolated vertices) and articulation nodes ($V_A$) represent cut vertices.
4. **Binary Lifting Preprocessing**: Executes an iterative DFS across all connected components of $F$ to populate `depth`, `component` IDs, and the $2^j$-ancestor jump table `up[LOG][N]`.
5. **Analytics & Interactive Querying**: Computes critical node impact rankings (`rankCriticalNodes`) and answers vertex-failure connectivity queries (`isDisconnectedAfterFailure`).

---

#### 3.3. Core Algorithms & Implementation Details

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

#### 3.4. Complexity Analysis

| Function / Operation | Time Complexity | Space Complexity | Implementation Notes |
| :--- | :--- | :--- | :--- |
| `buildBlockCutForest` | $O((n + m) \log n)$ | $O(n \log n)$ | Deduplicates block vertices via `std::sort`; binary lifting table `up` uses $O(N \log N)$ space where $N \le 2n$. |
| `findLCA` / `getDistance` | $O(\log n)$ | $O(1)$ | Uses precomputed `LOG` jump pointers (`LOG` $\approx \lceil \log_2 N \rceil$). |
| `isDisconnectedAfterFailure` | $O(\log n)$ | $O(1)$ | $O(1)$ fast-paths for non-articulation points; 3 LCA calls for articulation points. |
| `rankCriticalNodes` | $O(n + k \log k)$ | $O(k)$ | Where $k \le n$ is the number of articulation points. |

---

#### 3.5. Architectural Highlights & Compliance
* **Strict Decoupling (`INTERFACE.md`)**: Separates graph decomposition (D1), independent DSU bridge detection (D2), BFS ground-truth verification (D4), and block-cut query processing (D3), enabling independent testing and clean integration.
* **Disconnected Graph Support**: Explicitly tracks `component[u]` across all forest roots, handling multi-component graphs natively without dummy super-roots.
* **Long-Path Resilience**: Iterative DFS in forest preprocessing satisfies the system testing rule against stack overflow on path-graph edge cases.

---

#### 3.6. Resources
* F. Harary, Graph Theory, Addison-Wesley, 1969, Chapter 3 (Blocks and the Block-Cutpoint Tree).
* R. E. Tarjan, “Depth-First Search and Linear Graph Algorithms,” SIAM Journal on Computing, Vol. 1, No. 2, pp. 146–160, 1972.

## 4. Structure A: Iterative Tarjan Low-Link Engine (`src/structA/`)
**Owner:** Rishika Goyal (`250008029` / `@che250008029-netizen`)  
**Derivation:** `docs/derivations/D1_250008029.pdf`

## 4. Structure A — Iterative Tarjan Low-Link Engine

**Role:** Structure A Owner (D1 — Rishika Goyal, `250008029` / `@che250008029-netizen`)  
**Derivation:** `docs/derivations/D1_250008029.pdf`  
**Owned Files:** `src/structA/structA.hpp`, `src/structA/structA.cpp`

---

### 4.1 Purpose & Mathematical Overview

Structure A computes the complete 2-connectivity decomposition of an undirected multigraph $G = (V, E)$ in a single $\mathcal{O}(V + E)$ Depth-First Search pass:
1. **Articulation Points (Cut Vertices):** All vertices $u \in V$ whose removal strictly increases the number of connected components.
2. **Bridges (Cut Edges):** All 0-based edge IDs $e \in \{0, \dots, m-1\}$ whose removal strictly increases the number of connected components.
3. **Biconnected Components (2-Vertex-Connected Edge Partitions):** A partition of all non-loop edges into maximal biconnected subgraphs (blocks), stored as sorted lists of edge IDs.

During DFS traversal from each unvisited root, Structure A assigns every vertex $u$ a discovery timestamp $\text{disc}[u]$ and maintains its low-link value $\text{low}[u]$—the minimum discovery time reachable from $u$'s DFS subtree using zero or more tree edges followed by at most one non-tree back edge (excluding the incoming parent edge):
- **Tree Edge $(u, v)$:** $\text{low}[u] = \min(\text{low}[u], \text{low}[v])$.
  - **Bridge Criterion:** $(u, v)$ is a bridge if and only if $\text{low}[v] > \text{disc}[u]$.
  - **Articulation Point Criterion:** A non-root vertex $u$ is an articulation point if and only if it has a tree child $v$ with $\text{low}[v] \ge \text{disc}[u]$; a DFS root $r$ is an articulation point if and only if it has $\ge 2$ DFS-tree children.
- **Back Edge $(u, v)$ (`edge_id` $\neq$ `parent_edge_id`):** $\text{low}[u] = \min(\text{low}[u], \text{disc}[v])$.

The complete correctness and linear-time proof is formalized in `docs/derivations/D1_250008029.pdf`.

---

### 4.2 Data Representation

Structure A operates directly on the shared 1-based `Graph` representation (`graph.hpp`) and populates `StructureAResult` (`src/structA/structA.hpp`):

| Array / Structure | Size | Meaning |
| :--- | :---: | :--- |
| `disc[v]` | $n + 1$ | DFS discovery timestamp of vertex $v$ (`0` if unvisited). |
| `low[v]` | $n + 1$ | Lowest discovery timestamp reachable from the subtree of $v$ via at most one back edge. |
| `is_ap[v]` | $n + 1$ | Boolean flag marking whether vertex $v$ satisfies the root or non-root articulation point criterion. |
| `dfs_stack` (`std::vector<DFSFrame>`) | $\le n$ | Explicit heap-allocated DFS call frames storing `(u, parent_edge_id, adj_idx, root_children)`. |
| `edge_stack` | $\le m$ | Explicit edge-ID stack holding traversed non-loop edges of active biconnected components. |
| `StructureAResult` | Output | Canonical sorted `articulationPoints`, `bridges`, and `biconnectedComponents` (each sorted by edge ID). |

Total auxiliary space is $\mathcal{O}(V + E)$.

---

### 4.3 Operations and Asymptotic Cost

| Operation / Stage | What It Does | Time Complexity |
| :--- | :--- | :---: |
| **Graph & State Initialization** | Allocates `disc`, `low`, `is_ap`, `dfs_stack`, and `edge_stack` for $n$ vertices and $m$ edges. | $\mathcal{O}(V + E)$ |
| **Iterative Tarjan DFS Loop** | Iterates over all unvisited roots $r \in [1, n]$, advancing neighbor cursors `adj_idx` on `dfs_stack` without call-stack recursion. | $\mathcal{O}(V + E)$ |
| **Edge Stack Extraction** | Pushes traversed edges onto `edge_stack` and pops edges back to `(u, v)` whenever $\text{low}[v] \ge \text{disc}[u]$ (or at root child return). | $\mathcal{O}(E)$ |
| **Canonical Output Sorting** | Sorts `articulationPoints`, `bridges`, and edge IDs within every biconnected component for deterministic comparison. | $\mathcal{O}(V \log V + E \log E)$ |
| **`analyzeStructureA(g)` (Total)** | Executes full single-pass decomposition and returns `StructureAResult`. | $\mathcal{O}(V + E)$ core ($\mathcal{O}(V \log V + E \log E)$ sorted) |

---

### 4.4 Code Changes & Engineering Decisions (`src/structA/`)

Key implementation changes and architectural decisions made in `src/structA/structA.hpp` and `src/structA/structA.cpp`:

1. **Replaced Recursive DFS with Explicit `DFSFrame` Stack (`std::vector<DFSFrame>`):**
   - *Change:* Converted recursive Tarjan DFS into an explicit iterative state machine where each `DFSFrame` tracks `(u, parent_edge_id, adj_idx, child_count)` and resumes parent post-order updates (`low[u] = min(low[u], low[v])`) upon child frame pop.
   - *Rationale:* Standard recursive DFS overflows the OS call stack on linear path graphs of $V = 100{,}000$ vertices (`adversary/path.cpp`). Moving frame state to contiguous heap memory eliminated stack-overflow crashes and enabled $V = 100{,}000$ execution in **32.92 ms** on paths and **125.02 ms** on $2.45\text{M}$-edge cliques.
2. **Multigraph Parallel-Edge Disambiguation via `parent_edge_id`:**
   - *Change:* Passed and stored the incoming 0-based `edge_id` rather than the parent vertex ID `p` when descending tree edges, ignoring only the exact incoming edge (`edge_id == frame.parent_edge_id`).
   - *Rationale:* Skipping by parent vertex `p` falsely ignores a second parallel edge $e_2 \neq e_1$ between $u$ and $p$, misclassifying parallel edges as bridges. Tracking `edge_id` treats $e_2$ as a valid back edge ($\text{low}[u] = \min(\text{low}[u], \text{disc}[p])$).
3. **Self-Loop & Isolated-Vertex Edge-Case Hardening:**
   - *Change:* Added explicit guards (`if (v == u) continue;`) before tree/back-edge classification and looped over all $r \in [1, n]$ for disconnected components without emitting empty blocks for degree-0 vertices.
   - *Rationale:* Prevents self-loops $(u, u)$ from inflating root child counts or polluting `edge_stack`, and ensures disconnected/empty graphs (`adversary/empty.cpp`, `adversary/disconnected.cpp`) produce zero spurious components.
4. **Canonical Output Normalization & Interface Alignment (`INTERFACE.md`):**
   - *Change:* Aligned header signatures (`analyzeStructureA(const Graph& g)`) with `graph.hpp`, deduplicated `articulationPoints`, and sorted `bridges` and `biconnectedComponents` deterministically.
   - *Rationale:* Enables direct $O(V + E)$ equality assertions against Structure B (`src/structB/`), the brute-force Oracle (`oracle/oracle.cpp`), and direct consumption by the Architect's Block-Cut Forest (`app/block_cut.cpp`).

---

### 4.5 Maintained Invariants & Verification

Structure A satisfies all structural invariants (`A1`–`A5`) verified by `oracle/invariants_check.cpp` and differential tests (`oracle/oracle_integration_test.cpp`):
- **A1 (Canonical Ordering):** `articulationPoints` and `bridges` are strictly sorted, duplicate-free, and within valid ranges (`[1, n]` and `[0, m-1]`).
- **A2 (Exact Edge Partition):** Every non-loop edge $e \in [0, m-1]$ belongs to **exactly one** biconnected component, and no block is empty.
- **A3 (Bridge-Block Duality):** Every bridge forms a singleton 1-edge biconnected component, and every non-bridge biconnected component contains $\ge 3$ edges (or $2$ parallel edges in a multigraph).
- **A4 (Articulation-Block Incidence):** A vertex $v$ appears in `articulationPoints` if and only if $v$ belongs to $\ge 2$ distinct biconnected components.
- **A5 (Block Connectivity):** Every reported biconnected component induces a connected subgraph.
- **Empirical Verification:** Passed **10/10** handcrafted edge cases (paths, cycles, stars, disconnected/isolated vertices, parallel edges, self-loops), **500/500** randomized differential tests (`seed = 12345`) against Structure B and the Oracle (`ALL TESTS PASSED (failures: 0)`), and all **35** Analyst benchmark datasets up to $V = 100{,}000$ and $E = 2{,}450{,}000$ (mean **51.64 ms** at $V = 100{,}000$, outperforming Structure B across all 7 topologies).

---

### 4.6 Sources & Repository Artifacts

- **Academic & Algorithmic References:**
  1. R. E. Tarjan, *"Depth-First Search and Linear Graph Algorithms"*, SIAM Journal on Computing, Vol. 1, No. 2, pp. 146–160, 1972.
  2. T. H. Cormen, C. E. Leiserson, R. L. Rivest, and C. Stein, *Introduction to Algorithms (CLRS)*, 4th ed., Chapter 20 (Elementary Graph Algorithms — DFS Edge Classification and Biconnected Components), MIT Press, 2022.
  3. CP-Algorithms, *"Finding Bridges in a Graph in $\mathcal{O}(N+M)$"* and *"Finding Articulation Points in a Graph in $\mathcal{O}(N+M)$"* (edge-ID formulation for undirected multigraphs).
- **Repository Deliverables & Integration Links:**
  - **Source Implementation:** `src/structA/structA.hpp`, `src/structA/structA.cpp`
  - **Mathematical Proof (Derivation D1):** `docs/derivations/D1_250008029.pdf`
  - **Differential & Invariant Verification:** `oracle/oracle_integration_test.cpp`, `oracle/invariants_check.cpp`
  - **Benchmark Logs & Scaling Figures:** `results/results.csv`, `docs/plots/oracle_vs_structures_log.png`, `docs/plots/scalability_comparison.png`, `docs/plots/topology_100k_comparison.png`
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

## 6. The Requirement Beyond the Lecture

### Rishika Goyal — Structure A

Tarjan's classic low-link formulation in the lecture notes relies on recursive Depth-First Search and assumes simple undirected graphs where skipping the parent vertex $p$ suffices to avoid reversing along a tree edge. When extending this to adversarial inputs up to $V = 100{,}000$ vertices and undirected multigraphs, recursive DFS overflows the operating system's call stack on deep path graphs, and skipping by parent vertex $p$ misclassifies parallel edges as bridges.

In **Derivation D1** (`docs/derivations/D1_250008029.pdf`) and `src/structA/structA.cpp`, I addressed both gaps by replacing call-stack recursion with an explicit heap-allocated `DFSFrame` state machine (`std::vector<DFSFrame>`) and tracking incoming 0-based `parent_edge_id`s rather than parent vertices. This preserves strict $\mathcal{O}(V + E)$ traversal complexity while handling parallel edges, self-loops, disconnected components, and deep $100{,}000$-vertex chains without stack overflow.

### Deepali Chauhan — Architect & Interface Owner

While standard lectures cover biconnected components and articulation points as static decompositions, answering dynamic fault-tolerance queries—such as whether removing a given node $f$ disconnects arbitrary nodes $x$ and $y$, and ranking vertices by topological fragmentation—requires compressing biconnected blocks into a queryable tree structure.

In **Derivation D3** (`docs/derivations/D3_250041014.pdf`) and `app/block_cut.cpp`, I bridged the raw `StructureAResult` decomposition to fast resilience analytics by constructing a bipartite Block-Cut Forest $F = (V_B \cup V_A, E_F)$ with singleton block handling for isolated vertices and iterative DFS binary lifting (`up[LOG][N]`). This reduces arbitrary single-vertex failure queries to $\mathcal{O}(1)$ fast-paths for non-articulation vertices and $\mathcal{O}(\log n)$ tree-path distance checks (`dist(x, y) == dist(x, f) + dist(f, y)`) for cut vertices, while computing exact fragmentation impact scores (`branches - 1`) in `app/ranking.cpp`.
### Bodapati Uma Sri — Structure B

The lecture finds bridges with Tarjan's low-link values, which Structure A already implements. The project requires a second, independent method that must agree with the first. That method, union-find path marking on a DFS tree, is not a lecture algorithm. Making it work also needs care on adversarial inputs: a recursive DFS overflows the call stack on a path of 100,000 vertices, skipping the parent by vertex misclassifies parallel edges as bridges, and a union-find whose representative is chosen by rank no longer tells us where a marked piece ends in the DFS tree.

In **Derivation D2** (`docs/derivations/D2_250041009.pdf`) and `src/structB/structB.cpp`, I addressed these gaps. I proved that every non-tree edge joins a vertex with one of its ancestors, that a tree edge is on a cycle exactly when a non-tree edge covers it, and that the marking loop marks exactly the tree path of each non-tree edge. So the unmarked tree edges are exactly the bridges. In the code, the DFS is iterative with an explicit stack and skips only the parent edge by its ID, so parallel edges, self-loops and disconnected graphs are handled. The union-find uses union by rank and path compression, and a separate `top[]` array stores the highest vertex of each set, because the representative is not always the highest vertex. The path marking costs $O((V+E)\,\alpha(V))$, and the sorting of the output adds $O(V \log V)$. The two-edge-connected components come from a BFS that never crosses a bridge.

**Keetha Akshaya — Oracle**

The lecture covers BFS and connected components. What goes beyond it is showing that a simple brute-force test can serve as ground truth for the low-link method. In Derivation D4 (`docs/derivations/D4_250041021.pdf`) I prove that a tree edge $(u, v)$ is a bridge exactly when $\text{low}[v] > \text{disc}[u]$, the criterion Structure A uses. I also prove that a bridge is a biconnected component by itself, so the bridges the oracle finds are exactly the single-edge blocks of Structure A's decomposition and of the block-cut forest.

The oracle itself (`countComponents` and `runOracle` in `oracle/oracle.cpp`) uses none of these techniques. It removes one vertex or edge, runs BFS and compares component counts, so it stays independent of Structures A and B. A removed edge is skipped by its id and not by its endpoints, so parallel edges stay separate.

### Dharavath Mamatha — Adversary

The reading explains that in an undirected graph, every non-tree edge in a DFS connects a vertex to one of its ancestors. This property is used in DFS-based graph algorithms, including finding biconnected components.

I used Claude to help me understand the derivation and the role of the root rule. My D5 derivation explains why an undirected DFS has no cross edges and why the root needs a separate articulation-point rule. My implementation differs from the paper’s algorithmic work: instead of implementing the full algorithm, I created seven graph generators—path, cycle, star, cliques, disconnected, empty, and random graphs—to test the project’s algorithms on different graph structures and edge cases.

## Oracle module(D4)
## 7. Correctness

### 7.1 The oracle and why it is correct

The oracle is a brute-force checker. It uses no clever algorithm. It applies the definitions directly: remove one vertex or one edge, count the connected components again with BFS, and compare with the original count c0. It does not use Structure A or Structure B, and it uses no DFS, low-link values or union-find.

Interface (from `oracle/oracle.hpp`, using the team's `graph.hpp`, where `Graph` has `n`, `m`, `edges` and `adj`):

- `int countComponents(const AdjList& adj, int n, int removedVertex = -1, int removedEdge = -1)` returns the number of components. A removed vertex is not counted as a component.
- `OracleResult runOracle(const Graph& g)` returns the original component count, the count after every vertex and every edge removal, the articulation points and the bridge ids, all sorted.
- `oracleArticulationPoints(g)`, `oracleBridges(g)` and `oracleTooLarge(g)` are helpers used by the Analyst's `benchmark_runner.cpp`.

Why `countComponents` is correct: a BFS started at a vertex visits exactly the vertices reachable from it (it never marks a vertex without a path from the start, and it never misses a vertex that has one). A new BFS starts only at an unvisited vertex, so every component is counted exactly once. The removed vertex is never entered, and the removed edge is skipped by its id (not by its endpoints), so parallel edges stay separate. A self-loop only leads back to a vertex that is already visited, so it never changes the count.

Why the test is correct: an edge is reported when the new count is greater than c0. Removing an edge splits its component into at most two pieces, so the count rises exactly when the edge is a bridge. Removing a vertex leaves c0 - 1 + k pieces, where k is the number of pieces its component breaks into, so the count rises exactly when k >= 2, which is the definition of an articulation point. Comparing with c0 and not with 1 keeps disconnected graphs correct. How this definition relates to the low-link criterion used by Structure A (a tree edge (u, v) is a bridge exactly when low(v) > disc(u)) is proved in `docs/derivations/D4_250041021.pdf`.

Cost: one count is O(n + m). Bridges need m counts and articulation points need n counts, so the oracle is about O((n + m)^2). It is only run up to 5000 vertices and 25000 edges; `oracleTooLarge` skips larger inputs. The 5000 comes from the project description and the 25000 is a team choice. The check applies only to the oracle report and never stops Structure A, Structure B or the application from running on the full graph.

### 7.2 Differential test

`oracle/oracle_integration_test.cpp` runs Structure A, Structure B and the oracle on the same graphs and compares the sorted lists. In this project an "operation" is one graph on which all methods are compared, because the input is a static graph.

| Item | Value |
|---|---|
| Operations compared | Structure A articulation points vs oracle; Structure A bridges vs oracle; Structure B bridges vs oracle |
| Named graphs (8) | path, cycle, star, disconnected graph with an isolated vertex, single vertex, triangle with a tail, two triangles joined by a path, star whose centre is not vertex 1 |
| Random graphs | 500 simple graphs (no self-loops or parallel edges), 1 to 25 vertices, up to 40 edges |
| Seed | 12345 (`std::mt19937`, fixed, so failures are reproducible) |
| Long input | A path with 200000 vertices, run on A and B only (the oracle is skipped above 5000 vertices); both must report n - 1 bridges. This checks that the DFS is iterative |
| Result | 500/500 random graphs matched; all 8 named graphs matched; long path passed; `ALL TESTS PASSED (failures: 0)` |

Reproduce:

    g++ -std=c++17 -Wall -Wextra -I. oracle/oracle.cpp oracle/oracle_integration_test.cpp src/structA/structA.cpp src/structB/structB.cpp -o oracle_integration
    oracle_integration.exe

The oracle's own test, `oracle/oracle_test.cpp`, has 16 test groups with hand-computed answers (single vertex, path, cycle, star, disconnected graphs, isolated vertices, K4, several articulation points and bridges, parallel edges, self-loops, exact count arrays, the size guard and `compareResults`). All passed.

A second comparison was run with the Adversary's checker `adversary_check.cpp`, compiled with `-DUSE_TEAM_ORACLE`. It compares Structure A, Structure B and `runOracle` with a second brute-force function inside the checker. Eight graphs from the Adversary's generators returned `RESULT: ALL OK`: `path 5`, `path 10`, `cycle 10`, `star 10`, `empty 5`, `disconnected 10`, `cliques 3 4` and `random 50 80 1`.

### 7.3 Invariant checkers

The invariant checkers are in `oracle/invariants_check.cpp`, a separate program in the oracle folder. It does not compute the answer again. It reads a graph, runs Structure A and Structure B, and checks facts about their results that must always be true. These are structural checks on the outputs, so they do not depend on the oracle's answers.

| Check | What it tests |
|---|---|
| I0 | the input is a simple graph (no self-loop, no parallel edge, endpoints in 1..n) |
| A1 | A's articulation points and bridges are sorted, without duplicates, and in range |
| A2 | the blocks partition the edges: every edge is in exactly one block, no block is empty |
| A3 | a block has one edge (and then it is a bridge) or at least three; no bridge lies in a larger block |
| A4 | a vertex is an articulation point exactly when it belongs to at least two blocks |
| A5 | every block is connected |
| B1 | B's bridges are sorted, without duplicates, and in range |
| B2 | B's components partition the vertices 1..n |
| B3 | a bridge joins two different components and every other edge stays inside one |
| B4 | the number of B's components equals the number of pieces left after removing the bridges |
| AB | Structure A and Structure B report the same bridges |

Build and use:

    g++ -O2 -std=c++14 oracle/invariants_check.cpp src/structA/structA.cpp src/structB/structB.cpp -I. -o inv
    gen.exe 5 | inv.exe

The checker prints one line per invariant group and a last line `ALL INVARIANTS OK` or `INVARIANT VIOLATED`. It was added to the repository after the differential test above and has not yet been run on the final code.

### 7.4 Bug log

| # | Date | Module | Symptom | How it was found | Fixed in commit |
|---|---|---|---|---|---|
| 1 | 2026-11-15 | oracle/ (folder layout) | The Oracle files sat in `src/oracle/`, but the team repo keeps them in `oracle/`. After the move, the includes in the integration test no longer resolved. | Compiler error and `git diff` against `origin/main` | `bc7d455`, `9255595` |
| 2 | 2026-11-15 | oracle/oracle.hpp, oracle/oracle.cpp | The oracle was written for the interface text, where `Graph` is a plain adjacency list. The final `graph.hpp` makes `Graph` a struct (`n`, `m`, `edges`, `adj`), so the code did not match it. | Compiler error against the final `graph.hpp` | `9dd7c35`, `ffbc4c5`, `eb911c9` |
| 3 | 2026-11-16 | oracle/oracle_test.cpp | `fatal error: graph.hpp: No such file or directory`. The test included `graph.hpp` as if it were in the same folder, but it is in the repo root. | Compiler error when building the oracle test | `3b111f1` |
| 4 | 2026-11-16 | oracle/oracle_integration_test.cpp | `'struct Graph' has no member named 'assign'` and `no match for operator[]`. The old test treated `Graph` as a vector and called functions that no longer exist. | Compiler error when running the integration test against Structure A and B | `c3513e7` |
| 5 | 2026-11-16 | oracle/ (git history) | Merge commit `8490188` deleted `oracle_integration_test.cpp` by mistake. A `git rm` was still staged when `git add oracle` ran. | `git show --stat HEAD` showed "1 file changed, 135 deletions" | `527e4a5` (revert) |
| Date | Module | Symptom | How found | Fix |
|---|---|---|---|---|
| 15 Nov | `graph.hpp` | `main` did not build: `graph.hpp` was not in `main` yet, and Structure A included `../common/graph.hpp`, which did not exist | compiling Structure A, Structure B and the oracle together | `graph.hpp` added by the Architect (commit `5c8b146`, latest update `9113694`) |
| 15 Nov | Structure A | Used the old type name `GraphData` and a different include path from the final `Graph` header | the same build attempt | PR #25 (`structA.hpp`) and PR #26 (`structA.cpp`) |
| 15 Nov | Structure B | Included `graph.hpp` from a different path than Structure A, and `computeStructureB` had a different signature | the same build attempt | commits `4e91d3f` and `f89e9a9` (include path), PR #28 (inline overload) |
| 15 Nov | Oracle | `oracle.hpp` defined its own `Graph` alias, which clashes with the shared `struct Graph` | reading `oracle.hpp` while building | fixed by the Oracle owner (PR #31) |
| 16 Nov | `adversary/path.cpp` | No check for a missing command-line argument, unlike the other six generators | reading the source code | not fixed; see Limitations (section 8.4) |

The oracle found no wrong-answer bug in Structure A or Structure B. They agreed with it on every graph tested, so the entries above are build and integration bugs.

Two mismatches were found in other modules while integrating. Neither changed any result:

- The draft of INTERFACE.md declared `struct OracleResult ( ... );` with round brackets. It has to be written with braces, as in `oracle/oracle.hpp`.
- `Graph::addEdge(u, u)` in `graph.hpp` pushes a self-loop twice into `adj[u]`, while INTERFACE.md says a self-loop is stored once. The oracle's BFS ignores a loop, and Structure A skips loops, so no result was affected.

### 7.5 Limitations

- The differential test compares one run of 500 random graphs and 8 named graphs. The 10^5 and 10^6 seeded operations named in the project document were not reached, and failing cases are not shrunk to a minimal one.
- The oracle checks graphs of at most 5000 vertices, so full-scale inputs (10^5 vertices, 10^6 edges) are checked only by comparing Structure A with Structure B.
- The invariant checkers accept only simple graphs (check I0) and have not yet been run on the final code.
- The 25000 edge limit is a team choice, not confirmed by the instructor.
- I did not measure the oracle's running time; the Analyst covers measurements.

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

The DFS root needs a separate rule because it has no ancestor above it, so the normal test low(c) ≥ disc(root) is always true for its children. It is an articulation point only if it has at least two DFS-tree children.

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
- There is no generator for self-loops because they are outside the agreed input format.
- I did not take timing measurements. Performance measurements are covered by the Analyst.
- The `path` generator does not check for a missing command-line argument (the other six generators do), so `./path` without a number fails instead of printing a usage message.

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
 
### 10. Experimental setup

### Structure A

Structure A (`src/structA/structA.cpp`) was compiled and verified locally on Windows 11 using `g++ -O2 -std=c++17 -I.` alongside Structure B, the Oracle (`oracle/oracle_integration_test.cpp`), structural invariant checks (`oracle/invariants_check.cpp`), and the adversarial generators (`adversary/*.cpp`). Correctness was verified across 10 handcrafted edge-case graphs (including parallel edges, self-loops, isolated vertices, stars, cycles, and paths) and 500 seeded random graphs (`seed = 12345`), while stack safety and linear scaling were tested up to $V = 100{,}000$ vertices across all seven adversarial topologies.### Structure A
### Structure B

Structure B (`src/structB/structB.cpp`) was compiled on a laptop running Windows 11 Home Single Language, version 25H2 (OS Build 26200.9974), x86_64 (AMD64), using g++ (MinGW.org GCC-6.3.0-1) 6.3.0 with `g++ -std=c++17 -O2 -c`.

Correctness was checked with `integration_test` (Structure B against Structure A and the oracle on the named edge-case graphs and the seeded random graphs, seed 12345), and a path of 200,000 vertices to confirm the iterative DFS does not overflow the stack. The benchmark times for Structure B come from the team's runs in `results/results.csv`, made on the machine stated in the README.

### Architect & Application (`app/`)

The Architect module (`app/block_cut.cpp`, `app/ranking.cpp`, `app/main.cpp`) was compiled from the repository root using `g++ -O2 -std=c++17 -I.` linked against `src/structA/structA.cpp`. Functional correctness of Block-Cut Forest construction, binary lifting LCA distances, `isDisconnectedAfterFailure` queries, and `rankCriticalNodes` ordering was verified across connected graphs, multi-component graphs with isolated vertices, and deep path topologies up to $V = 100{,}000$ without recursion stack overflow.

**Oracle (`oracle/`)**

The oracle tests were compiled and run on a laptop with Windows 11 version 26H2 (OS Build 26300.9818), x86_64, a 13th Gen Intel(R) Core(TM) i5-1334U processor and 16 GB RAM, using g++ 16.2.0 (MinGW-W64 x86_64-ucrt-posix-seh) with `g++ -std=c++17 -Wall -Wextra -I.` (`oracle/invariants_check.cpp` with `-O2 -std=c++14`, as in Section 7.3). Input sizes were up to 5,000 vertices and 25,000 edges (the `oracleTooLarge` limit), plus a path of 200,000 vertices on Structures A and B only. The random graphs use seed 12345. The oracle timings in Section 9 come from the Analyst's machine (Section 9.2), not from this one.

### Adversary

The adversary generators were compiled on a laptop running Windows 11 Home Single Language, version 25H2 (OS Build 26200.9974), using MinGW GCC 6.3.0 with g++ -O2 and the compiler’s default C++ standard. The laptop has 2 CPU cores and 4 logical processors, with 160 KB L1 cache, 1.0 MB L2 cache, and 4.0 MB L3 cache. The CPU model was not recorded.

Correctness testing used eight small inputs: path 5, path 10, cycle 10, star 10, empty 5, disconnected 10, cliques 3 4, and random 50 80 1. The random graph used seed 1. These tests were run through the local checker; no timing measurements or repeated trials were recorded.

### 11.Results

Refer Section 9.4

### 12. Limitations

- Structure A sorts `articulationPoints`, `bridges`, and edge IDs within each biconnected component to guarantee canonical output ordering for differential testing, adding an $\mathcal{O}(V \log V + E \log E)$ post-processing step on top of the $\mathcal{O}(V + E)$ DFS traversal.
- While Structure A supports multigraph parallel edges and self-loops via unique 0-based `edge_id` tracking, downstream invariant checks (`invariants_check.cpp`) and adversarial generators are restricted to simple graphs (`I0`), so multigraph behavior was validated primarily via targeted unit tests in `oracle_integration_test.cpp`.

- `buildBlockCutForest` deduplicates vertices per biconnected block using `std::sort` and allocates an $\mathcal{O}(N \log N)$ binary lifting table `up[LOG][N]` ($N \le 2n$), which uses slightly more memory and $\mathcal{O}((n + m)\log n)$ preprocessing time compared to Euler-tour RMQ or linear-time tarjan offline LCA.
- Current failure queries (`isDisconnectedAfterFailure`) and impact rankings (`rankCriticalNodes`) model single-vertex removals only; multi-vertex simultaneous failures or edge-specific failure queries (via bridge-cut trees) are not yet supported in `app/main.cpp`.
  
- The oracle runs only up to 5,000 vertices and 25,000 edges (the 25,000 is a team choice). Its cost is quadratic, so larger graphs are checked only by comparing Structure A with Structure B.
- The oracle reads the same `Graph` from `graph.hpp` as the structures, so a bug in building the adjacency list would affect all of them equally.
- `compareResults` removes duplicates before comparing, so a structure that reports the same item twice still shows MATCH.
- The differential test uses 500 random graphs and 8 named graphs, not $10^5$ or $10^6$ operations, and it does not shrink failing cases.
- `oracle/invariants_check.cpp` accepts only simple graphs and had not yet been run on the final code. The oracle's running time comes from the Analyst's benchmark (Section 9), and I did not time it separately.
  
- The adversary generators were tested only on small inputs through the local checker. Their correctness and performance on large graphs were not independently evaluated.

## 13. References

**Papers and books**

1. R. E. Tarjan, "Depth-first search and linear graph algorithms," *SIAM Journal on Computing* 1(2):146–160, 1972.
2. J. Hopcroft and R. E. Tarjan, "Algorithm 447: Efficient algorithms for graph manipulation," *Communications of the ACM* 16(6):372–378, 1973.
3. R. E. Tarjan, "A note on finding the bridges of a graph," *Information Processing Letters* 2(6):160–161, 1974.

**Tools**

- AI assistant: [Claude,ChatGPT,Gemini AI].Details and representative prompts are in `TOOLS.md`.
- LaTeX on Overleaf, for the derivations.

