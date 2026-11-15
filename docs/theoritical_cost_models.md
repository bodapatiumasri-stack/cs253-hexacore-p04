# Derivation D6: Theoretical Cost Models & Performance Predictions

## 1. Asymptotic Complexity Models

### 1.1 Structure A: Iterative Tarjan Low-Link DFS
* **Time Complexity:** $\mathcal{O}(V + E)$
* **Space Complexity:** $\mathcal{O}(V + E)$
* **Theoretical Justification:** Structure A performs a single-pass Depth-First Search over the graph. Every vertex $v \in V$ is visited once, and each undirected edge $(u, v) \in E$ is traversed exactly twice (once from each endpoint). Tracking discovery times $\text{disc}[u]$ and low-link values $\text{low}[u]$ requires $\mathcal{O}(1)$ work per edge. Maintaining explicit stack frames and an edge stack for biconnected component extraction operates within a linear bound relative to graph size.

### 1.2 Structure B: Union-Find Path Marking
* **Time Complexity:** $\mathcal{O}((V + E)\alpha(V))$
* **Space Complexity:** $\mathcal{O}(V + E)$
* **Theoretical Justification:** Structure B constructs a DFS tree in $\mathcal{O}(V + E)$ time to separate tree edges from back-edges. For each back-edge, path marking traverses tree cycles and contracts paths using Disjoint-Set Union (DSU). With path compression and union by rank, each DSU operation runs in $\mathcal{O}(\alpha(V))$ amortized time, where $\alpha(V)$ is the Inverse Ackermann function. Since $\alpha(V) \le 4$ for all practical values of $V$, execution scales almost linearly.

### 1.3 Oracle Engine: Brute-Force BFS Baseline
* **Time Complexity:** $\mathcal{O}(V(V + E))$
* **Space Complexity:** $\mathcal{O}(V + E)$
* **Theoretical Justification:** The Oracle engine systematically tests for connectivity failures by temporarily masking each vertex $v \in V$ (and its incident edges) and running a full Breadth-First Search (BFS) of cost $\mathcal{O}(V + E)$ to count connected components. Repeating this traversal across all $V$ vertices results in a quadratic runtime bound.

---

## 2. Predicted Performance Separation Points

1. **Validation Range ($V \le 5,000$):** All three structures execute within acceptable time limits. The Oracle engine scales quadratically but remains computationally feasible for small graphs to establish ground-truth validation.
2. **Scalability Range ($V > 5,000$ to $V = 100,000$):** The execution time of the Oracle engine diverges quadratically ($\mathcal{O}(V^2 + VE)$), rendering it unusable for large networks. Structures A and B continue to scale linearly.
3. **Structure A vs. Structure B Comparison:** Structure B may have a slightly higher constant-factor overhead compared with Structure A due to:

* Multiple processing phases, including DFS construction, back-edge identification, cycle-edge marking, and BFS-based component identification.
* Indirect array accesses and path traversal during DSU-style path compression.

Both approaches aim for near-linear time complexity. The actual performance difference depends on graph characteristics and implementation details and can be evaluated through benchmarking.