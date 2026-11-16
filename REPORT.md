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

**Purpose.** Structure B finds all bridges of the graph by a different method from Structure A. It builds a DFS tree, then uses path marking with union-find to mark every tree edge that lies on a cycle. The unmarked tree edges are the bridges. It also lists the 2-edge-connected components (the pieces left after removing the bridges). The proof of correctness and of the running time is in my derivation D2 (`docs/derivations/D2_mc250041009.pdf`).

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

The two sorts only fix the output order so that it can be compared with Structure A and the oracle. The core of Structure B (finding the bridges) is O((V+E) α(V)). [If INTERFACE.md lists other operations for Structure B, add each one here with its exact name and cost.]

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

Derivation: D2 (`docs/derivations/D2_mc250041009.pdf`). Code: `structB.cpp`. Main commits / pull requests:
commits-
https://github.com/bodapatiumasri-stack/cs253-hexacore-p04/commits/f13237654d770ba8d73581227bac90415bb2ad45
https://github.com/bodapatiumasri-stack/cs253-hexacore-p04/commits/8556e74e079fe336323d5444975bce30b9be1c6a
https://github.com/bodapatiumasri-stack/cs253-hexacore-p04/commits/f89e9a96177c8f0fb126e6e9dc7f0e2244990c6f
https://github.com/bodapatiumasri-stack/cs253-hexacore-p04/commits/7ef8033f728ddf841726ecef6c6b7bf17badc644
https://github.com/bodapatiumasri-stack/cs253-hexacore-p04/commits/2c1b118c5426866405907c7254a8c03b235b12c1
pull requests-
https://github.com/bodapatiumasri-stack/cs253-hexacore-p04/pull/7
https://github.com/bodapatiumasri-stack/cs253-hexacore-p04/pull/14
https://github.com/bodapatiumasri-stack/cs253-hexacore-p04/pull/38

. Reading-list item and how my code differs from the paper: see Section 6.