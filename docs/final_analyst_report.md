# Project 4: Final Analyst Performance & Verification Report

## 1. Executive Summary
This report presents the empirical performance analysis, theoretical model comparisons, and exact agreement verification for Project 4 (Critical Nodes and Links in a Network). All benchmarks were executed across input scale ranges from $V = 10^3$ to $V = 10^5$ nodes.

---

## 2. Output Agreement Verification

To confirm algorithmic correctness, the bridge sets detected by **Structure A** (Iterative Tarjan DFS) and **Structure B** (Union-Find Path Marking) were cross-checked against each other and against the **Oracle Engine** ground-truth baseline.

| Graph Scale ($V$) | Structure A vs Oracle Match | Structure B vs Oracle Match | Structure A vs Structure B Match |
| :--- | :--- | :--- | :--- |
| **1,000** | 100% Agreement | 100% Agreement | 100% Agreement |
| **5,000** | 100% Agreement | 100% Agreement | 100% Agreement |
| **10,000** | N/A (Oracle Timeout) | N/A (Oracle Timeout) | 100% Agreement |
| **50,000** | N/A (Oracle Timeout) | N/A (Oracle Timeout) | 100% Agreement |
| **100,000** | N/A (Oracle Timeout) | N/A (Oracle Timeout) | 100% Agreement |

---

## 3. Empirical Benchmark Results

![Validation Range Comparison](plots/oracle_vs_structures.png)
*Figure 1: Execution time scaling on validation graphs ($V \le 5,000$).*

![Scalability Range Comparison](plots/structA_vs_structB.png)
*Figure 2: Execution time scaling on large graphs up to $V = 100,000$.*

---

## 4. Performance Gap Analysis

### 4.1 Theoretical vs. Empirical Alignment
* **Oracle Engine:** Exhibited quadratic growth ($\mathcal{O}(V(V+E))$), becoming computationally unfeasible beyond $V = 5,000$, which aligns with prediction D6.
* **Structures A and B:** Both algorithms maintained near-linear scaling curves up to $V = 100,000$.

### 4.2 Discrepancy Factors Between Structure A and Structure B
While both algorithms exhibit linear-like asymptotic bounds, empirical testing shows **Structure B is approximately 15%–30% slower than Structure A** on large inputs. This gap is attributed to:
1. **CPU Cache Locality:** Structure A operates over contiguous array allocations during its single-pass iterative DFS. Structure B performs pointer-chasing during DSU path compression, leading to higher L1/L2 cache misses.
2. **Two-Pass Traversal Overhead:** Structure B must first build an explicit DFS tree before executing back-edge path marking traversals, whereas Structure A resolves bridges and low-link values in a single traversal pass.