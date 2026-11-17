import os
import pandas as pd
import matplotlib.pyplot as plt

script_dir = os.path.dirname(os.path.abspath(__file__))
csv_path = os.path.join(script_dir,"..", "results", "results.csv")

df = pd.read_csv(csv_path)

df = df[df["V"].astype(str) != "V"].copy()
for col in ["V", "E", "StructA_ms", "StructB_ms", "Oracle_ms"]:
    df[col] = pd.to_numeric(df[col], errors="coerce")

df["Filename"] = df["Graph_Path"].apply(lambda p: os.path.basename(str(p)))
df["Topology"] = df["Filename"].apply(lambda f: f.split("_")[0])


df_sorted = df.sort_values("V")
scale_summary = df_sorted.groupby("V")[["StructA_ms", "StructB_ms"]].mean().reset_index()

plt.figure(figsize=(9, 5))
plt.plot(scale_summary["V"], scale_summary["StructA_ms"], marker="s", linewidth=2, color="blue", label="Structure A (Iterative Tarjan DFS)")
plt.plot(scale_summary["V"], scale_summary["StructB_ms"], marker="^", linewidth=2, color="green", label="Structure B (Union-Find Path Marking)")
plt.title("Scalability Comparison Across Graph Sizes (Mean Runtime over 7 Topologies)")
plt.xlabel("Number of Vertices (V)")
plt.ylabel("Execution Time (ms)")
plt.grid(True, linestyle="--", alpha=0.6)
plt.legend()
plt.tight_layout()
plt.savefig(os.path.join(script_dir, "scalability_comparison.png"), dpi=300)
plt.close()

df_100k = df[df["V"] >= 90000].sort_values("Topology")
if not df_100k.empty:
    x = range(len(df_100k))
    width = 0.35

    plt.figure(figsize=(10, 5))
    plt.bar([i - width/2 for i in x], df_100k["StructA_ms"], width=width, color="royalblue", label="Structure A (Iterative Tarjan DFS)")
    plt.bar([i + width/2 for i in x], df_100k["StructB_ms"], width=width, color="seagreen", label="Structure B (Union-Find Path Marking)")
    plt.xticks(list(x), df_100k["Topology"], rotation=20)
    plt.title("Adversarial Topology Runtime Comparison at V ≈ 100,000")
    plt.xlabel("Graph Topology")
    plt.ylabel("Execution Time (ms)")
    plt.grid(True, axis="y", linestyle="--", alpha=0.6)
    plt.legend()
    plt.tight_layout()
    plt.savefig(os.path.join(script_dir, "topology_100k_comparison.png"), dpi=300)
    plt.close()

df_oracle = df[df["Oracle_ms"] > 0].groupby("V")[["StructA_ms", "StructB_ms", "Oracle_ms"]].mean().reset_index()
if not df_oracle.empty:
    plt.figure(figsize=(9, 5))
    plt.plot(df_oracle["V"], df_oracle["Oracle_ms"], marker="o", linewidth=2, color="crimson", label="Oracle O(V(V+E)) BFS Baseline")
    plt.plot(df_oracle["V"], df_oracle["StructA_ms"], marker="s", linewidth=2, color="blue", label="Structure A O(V+E)")
    plt.plot(df_oracle["V"], df_oracle["StructB_ms"], marker="^", linewidth=2, color="green", label="Structure B O((V+E) α(V))")
    plt.yscale("log")
    plt.title("Verification Baseline: Oracle vs. Linear-Time Structures (Log Scale)")
    plt.xlabel("Number of Vertices (V)")
    plt.ylabel("Execution Time (ms, Log Scale)")
    plt.grid(True, which="both", linestyle="--", alpha=0.6)
    plt.legend()
    plt.tight_layout()
    plt.savefig(os.path.join(script_dir, "oracle_vs_structures_log.png"), dpi=300)
    plt.close()

print("Successfully generated all 3 benchmark charts in benchmark/!")
