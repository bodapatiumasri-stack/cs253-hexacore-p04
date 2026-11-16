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
