"""
Analiza benchmark pentru rularile Game of Life MPI.

Ce face scriptul:
- citeste CSV-ul produs de simulari;
- separa rularile de strong scaling si weak scaling;
- calculeaza speedup si efficiency pentru strong scaling;
- salveaza tabelele rezultate si graficele in output/.

Scriptul este intentionat simplu: citeste datele, calculeaza indicatorii si
genereaza graficele fara sa modifice datele de baza.
"""

import os

import pandas as pd
import matplotlib.pyplot as plt

CSV_PATH = "output/benchmarks/benchmark_results.csv"
GRAPH_DIR = "output/graphs"

# Cream directorul pentru grafice daca nu exista deja.
os.makedirs(GRAPH_DIR, exist_ok=True)

# incarcam toate rezultatele benchmark din CSV.
df = pd.read_csv(CSV_PATH)

# Pastram separat rularile pentru strong scaling si weak scaling.
strong_df = df[df["run_name"].str.startswith("strong")].copy()
weak_df = df[df["run_name"].str.startswith("weak")].copy()

# Ordinea corecta a punctelor in grafic conteaza, deci sortam dupa numarul de procese.
strong_df = strong_df.sort_values("processes")
weak_df = weak_df.sort_values("processes")

# t1 este timpul de referinta pentru 1 proces MPI.
t1 = strong_df[strong_df["processes"] == 1]["total_time"].iloc[0]

# Speedup = timpul pe 1 proces / timpul pe N procese.
strong_df["speedup"] = t1 / strong_df["total_time"]

# Efficiency = speedup / numarul de procese.
strong_df["efficiency"] = strong_df["speedup"] / strong_df["processes"]

# Salvam tabelele finale ca referinta pentru analiza.
strong_df.to_csv("output/benchmarks/strong_scaling_analysis.csv", index=False)

weak_df.to_csv("output/benchmarks/weak_scaling_analysis.csv", index=False)

# Graficul speedup compara valorile masurate cu idealul liniar.
plt.figure()
plt.plot(
    strong_df["processes"], strong_df["speedup"], marker="o", label="Measured speedup"
)

plt.plot(
    strong_df["processes"],
    strong_df["processes"],
    linestyle="--",
    label="Ideal speedup",
)
plt.xlabel("Number of MPI processes")
plt.ylabel("Speedup")
plt.title("Strong Scaling - Speedup")
plt.legend()
plt.grid(True)
plt.savefig(f"{GRAPH_DIR}/speedup.png", dpi=300)
plt.close()

# Graficul efficiency arata cat de bine se folosesc procesele MPI.
plt.figure()
plt.plot(strong_df["processes"], strong_df["efficiency"], marker="o")
plt.xlabel("Number of MPI processes")
plt.ylabel("Parallel efficiency")
plt.title("Strong Scaling - Efficiency")
plt.grid(True)
plt.savefig(f"{GRAPH_DIR}/efficiency.png", dpi=300)
plt.close()

# Aici comparam timpul de calcul cu timpul de comunicatie.
plt.figure()
plt.plot(
    strong_df["processes"],
    strong_df["computation_time"],
    marker="o",
    label="Computation time",
)

plt.plot(
    strong_df["processes"],
    strong_df["communication_time"],
    marker="o",
    label="Communication time",
)
plt.xlabel("Number of MPI processes")
plt.ylabel("Time (seconds)")
plt.title("Computation vs Communication Time")
plt.legend()
plt.grid(True)
plt.savefig(f"{GRAPH_DIR}/communication_vs_computation.png", dpi=300)
plt.close()

# Weak scaling: urmarim cum evolueaza timpul total cand cresc si problema si numarul de procese.
plt.figure()

plt.plot(weak_df["processes"], weak_df["total_time"], marker="o")

plt.xlabel("Number of MPI processes")
plt.ylabel("Execution time (seconds)")
plt.title("Weak Scaling")

plt.grid(True)

plt.savefig(f"{GRAPH_DIR}/weak_scaling.png", dpi=300)

plt.close()

print("Benchmark analysis generated:")
print("output/benchmarks/strong_scaling_analysis.csv")
print("output/benchmarks/weak_scaling_analysis.csv")
print("output/graphs/weak_scaling.png")
print("output/graphs/speedup.png")
print("output/graphs/efficiency.png")
print("output/graphs/communication_vs_computation.png")
