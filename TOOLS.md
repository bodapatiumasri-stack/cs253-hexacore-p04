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

## Representative Prompts

The prompts below are taken from my own messages. Only spelling, capital letters and punctuation were corrected; the wording and the meaning are the same as I wrote them.

1. **28 Oct 2026:** "So yeah, we have a DSA project. Mine is Critical Nodes and Links in a Network. So tell me once what it is trying to say, I mean everything: the guidelines and my project topic. Then we will work on that."

2. **14 Nov 2026 (D5 derivation):** "So can you give me the proof in simple words, without using complicated words? And also, it is a 3-page PDF. Will it be okay?"

3. **15 Nov 2026 (checking my generators with the team's code):** "She uploaded graph.hpp, so tell me step by step what to do."
#### Rishika Goyal (250008029)

1. **Nov 2026 (Iterative Tarjan & Edge Cases):** "How do I convert Tarjan's DFS into an iterative version using an explicit stack so it handles parallel edges via `edge_id`, self-loops, and isolated vertices cleanly?"
2. **Nov 2026 (Header & Interface Alignment):** "How should I align `analyzeStructureA` in `structA.hpp` and `structA.cpp` with `graph.hpp` so it works cleanly with both 1-argument and 2-argument calls without merge conflicts?"
3. **Nov 2026 (Verification & Live Modifications):** "Walk me through how to verify Structure A against the brute-force Oracle and Structure B, and practice modifying `structA.cpp` to extract BCC vertices and component split counts."


| Claude (Anthropic; model version not recorded) | Akshaya (250041021) | Oracle design and BFS understanding (7–10 Nov 2026) | None; used for learning | Understood brute-force checking using BFS and connected components. |
| Claude (Anthropic; model version not recorded) | Akshaya (250041021) | Oracle implementation (10–15 Nov 2026) | `oracle/oracle.cpp`, `oracle/oracle.hpp` | Reviewed vertex removal, edge removal and component counting. |
| Claude (Anthropic; model version not recorded) | Akshaya (250041021) | Interface design and integration (15–16 Nov 2026) | `oracle/oracle.hpp` | Reviewed function signatures, output format and comparison with Structure A and Structure B. |
| Claude (Anthropic; model version not recorded) | Akshaya (250041021) | Testing and complexity review (16 Nov 2026) | `oracle/oracle_integration_test.cpp` | Reviewed edge cases and verified that 500 random stress-test graphs matched. |

## Representative Prompts

The following prompts are representative of my usage. Only spelling, capital letters and punctuation were corrected.

1. **7 Nov 2026:** "Design a brute-force Oracle using BFS to identify articulation points and bridges by removing vertices and edges and comparing connected component counts. Include edge cases and time complexity."

2. **15–16 Nov 2026:** "Review the Oracle implementation for correctness, including disconnected graphs and edge IDs. Explain its integration with Structure A and Structure B, suitable function signatures and output format."