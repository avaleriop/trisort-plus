import array
import random
import time
import sys
import pathlib
import numpy as np

# Asegura que cargue trisortc.so desde build/
sys.path.insert(0, str(pathlib.Path(__file__).parent))
import trisortc

def benchmark_trisort_c(n):
    data = array.array("i", (random.randrange(1_000_000) for _ in range(n)))
    t0 = time.perf_counter()
    trisortc.trisort_inplace(data)
    return time.perf_counter() - t0

def benchmark_sorted(n):
    data = [random.randrange(1_000_000) for _ in range(n)]
    t0 = time.perf_counter()
    _ = sorted(data)
    return time.perf_counter() - t0

def benchmark_numpy(n):
    data = np.random.randint(0, 1_000_000, size=n, dtype=np.int32)
    t0 = time.perf_counter()
    _ = np.sort(data)
    return time.perf_counter() - t0

def run_all():
    sizes = [10_000, 100_000, 1_000_000]
    results = {}
    for n in sizes:
        print(f"==== Tamaño: {n} ====")
        t_trisort = benchmark_trisort_c(n)
        t_sorted = benchmark_sorted(n)
        t_numpy = benchmark_numpy(n)
        results[n] = (t_trisort, t_sorted, t_numpy)
        print(f"TriSort+ C/OpenMP: {t_trisort:.6f} s")
        print(f"sorted():          {t_sorted:.6f} s")
        print(f"np.sort():         {t_numpy:.6f} s\n")
    # Guardar en CSV
    with open("benchmark_results.csv", "w") as f:
        f.write("n,TriSortC,Sorted,NumpySort\n")
        for n, (t1, t2, t3) in results.items():
            f.write(f"{n},{t1:.6f},{t2:.6f},{t3:.6f}\n")
    print("Resultados guardados en benchmark_results.csv")

if __name__ == "__main__":
    run_all()
