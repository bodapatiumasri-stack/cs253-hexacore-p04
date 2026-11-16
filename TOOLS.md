# Tools Used

This file lists every AI tool or other significant tool used in this project, on which parts, and what was changed or verified afterwards. Each member adds their own rows to the table below, with their name in the "Used by" column.

| Tool and version | Used by | Used for | Files affected | What we changed or verified |
|---|---|---|---|---|
| Claude (Anthropic; model version not recorded) | Mamatha (250041015) | Understanding the project, DFS, low-link values, the root rule, and Git/GitHub steps (28 Oct - 16 Nov 2026) | None; used for learning | Read the explanations and followed the examples. |
| Claude (Anthropic; model version not recorded) |Mamatha (250041015) | Generating the first versions of the seven adversarial graph generators (Nov 2026) | `adversary/path.cpp`, `adversary/cycle.cpp`, `adversary/star.cpp`, `adversary/cliques.cpp`, `adversary/random.cpp`, `adversary/disconnected.cpp`, `adversary/empty.cpp` | No changes to the generated code. The generators were compiled and run on small inputs, and the outputs were checked against the expected graphs. |
| Claude (Anthropic; model version not recorded) | Mamatha (250041015) | Preparing the first draft of the D5 derivation text and its LaTeX code (Nov 2026) | `docs/derivations/D5_250041015.pdf` | The PDF was built in Overleaf and the proof steps were read through. No changes to the text. |
| Claude (Anthropic; model version not recorded) |Mamatha (250041015) | Creating a local test helper (`adversary_check.cpp`) and testing the generators against copies of Structure A, Structure B, a brute-force checker, and a deliberately modified Structure A (15-16 Nov 2026) | `adversary_check.cpp` (local file, not committed to the repository) | The first test runs were performed in Claude's environment. On 16 Nov, a teammate compiled the helper with the team's Structure A, Structure B and Oracle on her laptop and ran my seven generators through it: eight small test cases returned `RESULT: ALL OK` (results in REPORT section 8.3). I could not run it on my own laptop because Windows blocked the programs. The helper produced an unused-parameter warning for `argv`; the team's source files produced no warnings. |
| Gemini (Google) | Rishika Goyal (250008029) | Understanding Tarjan's DFS invariants (`disc`, `low`, root rule, edge-stack BCC extraction), Structure A correctness proofs (Nov 2026) | None; used for conceptual learning and derivation practice | Read explanations, worked through edge-case traces manually, and practiced deriving Structure A invariants from scratch. |
| Gemini (Google) | Rishika Goyal (250008029) | Converting recursive Tarjan's DFS into an iterative `DFSFrame` state-machine implementation and handling multigraph edge cases (`edge_id` tracking, self-loops, isolated vertices) (Nov 2026) | `src/structA/structA.hpp`, `src/structA/structA.cpp` | Reviewed and adapted the iterative stack unwinding logic, added `std::sort` for deterministic output, compiled with `g++ -std=c++17 -O2 -Wall -Wextra` with zero warnings, and verified against `oracle.cpp` and `structB.cpp`. |
| Gemini (Google) | Rishika Goyal (250008029) | Reconciling `analyzeStructureA` function signatures and Common Graph Interface (`graph.hpp`) headers during team branch integration (Nov 2026) | `src/structA/structA.hpp`, `src/structA/structA.cpp`, `graph.hpp` | Resolved header merge conflicts, verified 1-based indexing and `Edge { to, id }` compliance, and tested end-to-end compilation with teammate modules. |
| ChatGPT (OpenAI; model version not recorded)| Deepali Chauhan(250041014)|Understanding Tarjan's algorithm, articulation points, bridges, biconnected components, Block-Cut Forest, and preparing the D3 derivation and its LaTeX code|None; used for learning and derivation practice|Read explanations, worked through graph examples, understood the algorithm, and prepared the derivation with mathematical explanations.|
|ChatGPT (OpenAI; model version not recorded)| Deepali Chauhan(250041014)|Understanding and checking the common Graph Interface and how it connects with other graph modules | `graph.hpp`, `INTERFACE.md`|Checked interface requirements, vertex indexing, edge structure, and compatibility with other modules.|
|ChatGPT (OpenAI; model version not recorded)| Deepali Chauhan(250041014)|Implementing the Block-Cut Forest using articulation points and biconnected components|`block_cut.h`,`block_cut.cpp`,`main.cpp`|Implemented the forest construction and vertex mapping, and tested connectivity queries on different graphs.|
|ChatGPT (OpenAI; model version not recorded)| Deepali Chauhan(250041014)|Implementing Critical Node Ranking and checking connectivity after removing a vertex|`ranking.h`,`ranking.cpp`|Checked the ranking logic and tested different graph cases to find possible errors.|
| Gemini (Google) | Sahasra (250041023) | Writing and refining the C++11 benchmark harness (`benchmark_runner.cpp`) to time Structure A, Structure B, and the Oracle baseline across all 35 adversarial datasets and verify exact output agreement (Nov 2026) | `benchmark/benchmark_runner.cpp`, `results/results.csv` | Adapted file-path handling and `std::chrono::steady_clock` timing wrappers, enforced the $V \le 5,000$ / $E \le 50,000$ Oracle cutoff, compiled with `g++ -std=c++11 -O2`, and verified 100% bridge/AP agreement (`PASS`) across all validation graphs. |
| Gemini (Google) | Sahasra (250041023) | Developing the Python visualization script (`plot_results.py`) using `pandas` and `matplotlib` to generate log-scale validation, scalability, and 100k topology comparison plots (Nov 2026) | `benchmark/plot_results.py`, `docs/plots/oracle_vs_structures_log.png`, `docs/plots/scalability_comparison.png`, `docs/plots/topology_100k_comparison.png` | Executed the script locally against `results.csv` and visually verified axis scaling, legends, labels, and PNG rendering quality across all three plots. |
| Gemini (Google) | Sahasra (250041023) | Drafting and structuring the D6 scaling/crossover derivation (`D6_250041023.pdf`) and synthesizing the empirical tables and architectural analysis in the final analyst report (Nov 2026) | `docs/derivations/D6_250041023.pdf`, `docs/final_analyst_report.md` | Verified all asymptotic bounds, speedup ratios, and runtime numbers directly against `results.csv` and formatted the derivation into a clean LaTeX PDF. |
| Gemini (Google) | Sahasra (250041023) | Resolving Git branch divergence, merge conflicts, and Pull Request integration workflows between `analyst` and `main` (Nov 2026) | `TOOLS.md` | Followed Git/GitHub branch synchronization steps and verified clean commit history prior to merging. |
| Claude (Anthropic; model version not recorded) | Akshaya (250041021) | Oracle design and BFS understanding (7–10 Nov 2026) | None; used for learning | Understood brute-force checking using BFS and connected components. |
| Claude (Anthropic; model version not recorded) | Akshaya (250041021) | Oracle implementation (10–15 Nov 2026) | `oracle/oracle.cpp`, `oracle/oracle.hpp` | Reviewed vertex removal, edge removal and component counting. |
| Claude (Anthropic; model version not recorded) | Akshaya (250041021) | Interface design and integration (15–16 Nov 2026) | `oracle/oracle.hpp` | Reviewed function signatures, output format and comparison with Structure A and Structure B. |
| Claude (Anthropic; model version not recorded) | Akshaya (250041021) | Testing and complexity review (16 Nov 2026) | `oracle/oracle_integration_test.cpp` | Reviewed edge cases and verified that 500 random stress-test graphs matched. |

| ChatGPT (OpenAI; GPT-6) | Bodapati Uma Sri (250041009) | Understanding Structure B, including iterative DFS, parent edges, back edges, ancestor relationships, bridge detection, and two-edge-connected components (Nov 2026) | None; used for conceptual learning and code walkthroughs | Read the explanations, traced the algorithm step by step, and worked through the DFS traversal and cycle-marking logic. |
| ChatGPT (OpenAI; GPT-6) | Bodapati Uma Sri (250041009) | Understanding and reviewing the Structure B implementation, including DSU, path compression, union by rank, top metadata, and complexity analysis (Nov 2026) | `src/structB/structB.cpp`, `src/structB/structB.hpp` | Discussed implementation decisions, examined the DSU-based path-marking approach, and revised the implementation. The final revised version was not compiled or tested by ChatGPT. |
| ChatGPT (OpenAI; GPT-6) | Bodapati Uma Sri (250041009) | Preparing the D2 correctness and complexity derivation for Structure B, including why path marking identifies tree edges that lie on cycles (Nov 2026) | `docs/derivations/D2_250041009.pdf` | Used explanations and proof guidance to develop the correctness argument, refine the lemma statements, and understand the complexity analysis. |


## Representative Prompts

#### Dharavath Mamatha (250041015)

The prompts below are taken from my own messages. Only spelling, capital letters and punctuation were corrected; the wording and the meaning are the same as I wrote them.

1. **28 Oct 2026:** "So yeah, we have a DSA project. Mine is Critical Nodes and Links in a Network. So tell me once what it is trying to say, I mean everything: the guidelines and my project topic. Then we will work on that."

2. **14 Nov 2026 (D5 derivation):** "So can you give me the proof in simple words, without using complicated words? And also, it is a 3-page PDF. Will it be okay?"

3. **15 Nov 2026 (checking my generators with the team's code):** "She uploaded graph.hpp, so tell me step by step what to do."
#### Rishika Goyal (250008029)

1. **Nov 2026 (Iterative Tarjan DFS & Multigraph Edge Cases):** "How can Tarjan's DFS algorithm be implemented iteratively using an explicit stack so it avoids recursion overflow on deep graphs while handling parallel edges via `edge_id`, self-loops, and isolated vertices?"
2. **Nov 2026 (Common Graph Interface Alignment):** "How should `analyzeStructureA` in `structA.hpp` and `structA.cpp` be structured to match the `Graph` adjacency list interface in `graph.hpp` and avoid header merge conflicts?"
3. **Nov 2026 (Determinism & Cross-Module Verification):** "How can I guarantee deterministic output ordering for articulation points, bridges, and biconnected component edge lists in Structure A so they match the Oracle and Structure B?"

### Deepali Chauhan (250041014)

1. **28 Oct,2026 (Understanding Algorithms and D3 Derivation):** "Explain Tarjan's algorithm, articulation points, bridges, biconnected components, and Block-Cut Forest in simple words with examples. Also help me understand the D3 derivation step by step and prepare its mathematical explanation and LaTeX code."
2. **2 Nov,2026(Common Graph Interface):**  "Help me understand the common Graph Interface and explain how my implementation should follow it, including vertex indexing, edge IDs, and compatibility with other graph modules."
3. **13 Nov,2026(Block-Cut Forest Implementation):** "Help me implement the Block-Cut Forest using articulation points and biconnected components. Explain how to create the forest, map original vertices to forest nodes, and answer connectivity queries."
4. **15 Nov,2026(Critical Node Ranking):** "Help me implement Critical Node Ranking and check whether two vertices remain connected after removing a given vertex. Explain the logic step by step and help me test it using different graph examples."

#### Keetha Akshaya(250041021)

The following prompts are representative of my usage. 

1. **7 Nov 2026:** "Design a brute-force Oracle using BFS to identify articulation points and bridges by removing vertices and edges and comparing connected component counts. Include edge cases and time complexity."

2. **15–16 Nov 2026:** "Review the Oracle implementation for correctness, including disconnected graphs and edge IDs. Explain its integration with Structure A and Structure B, suitable function signatures and output format."
3. **Nov 2026 (Determinism & Cross-Module Verification):** "How can I guarantee deterministic output ordering for articulation points, bridges, and biconnected component edge lists in Structure A so they match the Oracle and Structure B
#### Sahasra (250041023)

1. **Nov 2026 (Benchmark Harness & Oracle Verification):** "How do I structure `benchmark_runner.cpp` in C++11 using `std::chrono::steady_clock` to benchmark Structure A, Structure B, and the Oracle across all 35 adversarial datasets, verify exact output agreement, and skip the Oracle when $V > 5,000$ or $E > 50,000$?"
2. **Nov 2026 (Performance Plotting & Visualization):** "Write a Python script using `pandas` and `matplotlib` to read `results.csv` and generate clean log-scale validation, scalability, and 100k topology comparison plots."
3. **Nov 2026 (Final Report & D6 Architectural Synthesis):** "Generate the complete empirical benchmark tables and architectural crossover analysis for `final_analyst_report.md` and `D6_250041023.pdf` directly from my updated `results.csv`."
#### Bodapati Uma Sri(250041009)
1. **Nov 2026 (Understanding DFS and Ancestor Relationships):** "Why do we use `tin` and `tout` to check whether one vertex is an ancestor of another? How does this help us distinguish back edges from other edges?"

2. **Nov 2026 (Handling Undirected Graphs and Parallel Edges):** "Since every undirected edge appears twice in the adjacency list, why do we skip the parent edge using its edge ID instead of simply checking the parent vertex? How does this handle parallel edges?"

3. **Nov 2026 (Understanding the Path-Marking Approach):** "When a back edge connects a descendant to an ancestor, why do we mark all the tree edges along the path between them? How does this help us identify bridges?"

4. **Nov 2026 (DSU Optimization and Complexity):** "If we use only path compression, can we claim the inverse-Ackermann complexity? Why do we need union by rank along with path compression, and how should we maintain the `top` vertex when two sets are merged?"
