import pandas as pd
import matplotlib.pyplot as plt

# Lee el CSV generado en benchmarks/ (o en build/ si lo mueves allí después)
df = pd.read_csv("benchmark_results.csv")

# Plot en escala log-log
plt.figure(figsize=(6, 4))
for col in ["TriSortC", "Sorted", "NumpySort"]:
    plt.plot(df["n"], df[col], marker="o", label=col)
plt.xscale("log")
plt.yscale("log")
plt.xlabel("Tamaño del dataset (n)")
plt.ylabel("Tiempo (s)")
plt.title("Benchmark comparativo: TriSort+ C/OpenMP vs sorted() vs np.sort()")
plt.legend()
plt.grid(True, which="both", ls="--", lw=0.5)
plt.tight_layout()
plt.savefig("benchmark_plot_final.png", dpi=300)
print("Gráfico guardado en benchmark_plot_final.png")

