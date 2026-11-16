# Tools Used

This file lists every AI tool or other significant tool used in this project, on which parts, and what was changed or verified afterwards. Each member adds their own rows to the table below, with their name in the "Used by" column.

| Tool and version | Used by | Used for | Files affected | What we changed or verified |
|---|---|---|---|---|
| Claude (Anthropic; model version not recorded) | Mamatha (250041015) | Understanding the project, DFS, low-link values, the root rule, and Git/GitHub steps (28 Oct - 16 Nov 2026) | None; used for learning | Read the explanations and followed the examples. |
| Claude (Anthropic; model version not recorded) |Mamatha (250041015) | Generating the first versions of the seven adversarial graph generators (Nov 2026) | `adversary/path.cpp`, `adversary/cycle.cpp`, `adversary/star.cpp`, `adversary/cliques.cpp`, `adversary/random.cpp`, `adversary/disconnected.cpp`, `adversary/empty.cpp` | No changes to the generated code. The generators were compiled and run on small inputs, and the outputs were checked against the expected graphs. |
| Claude (Anthropic; model version not recorded) | Mamatha (250041015) | Preparing the first draft of the D5 derivation text and its LaTeX code (Nov 2026) | `docs/derivations/D5_250041015.pdf` | The PDF was built in Overleaf and the proof steps were read through. No changes to the text. |
| Claude (Anthropic; model version not recorded) |Mamatha (250041015) | Creating a local test helper (`adversary_check.cpp`) and testing the generators against copies of Structure A, Structure B, a brute-force checker, and a deliberately modified Structure A (15-16 Nov 2026) | `adversary_check.cpp` (local file, not committed to the repository) | The first test runs were performed in Claude's environment. On 16 Nov, a teammate compiled the helper with the team's Structure A, Structure B and Oracle on her laptop and ran my seven generators through it: eight small test cases returned `RESULT: ALL OK` (results in REPORT section 8.3). I could not run it on my own laptop because Windows blocked the programs. The helper produced an unused-parameter warning for `argv`; the team's source files produced no warnings. |

## Representative Prompts

The prompts below are taken from my own messages. Only spelling, capital letters and punctuation were corrected; the wording and the meaning are the same as I wrote them.

1. **28 Oct 2026:** "So yeah, we have a DSA project. Mine is Critical Nodes and Links in a Network. So tell me once what it is trying to say, I mean everything: the guidelines and my project topic. Then we will work on that."

2. **14 Nov 2026 (D5 derivation):** "So can you give me the proof in simple words, without using complicated words? And also, it is a 3-page PDF. Will it be okay?"

3. **15 Nov 2026 (checking my generators with the team's code):** "She uploaded graph.hpp, so tell me step by step what to do."