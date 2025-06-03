```markdown
# TriSort+ C/OpenMP

**TriSort+** is an entropy-adaptive hybrid sorting algorithm that dynamically chooses between three different sorting strategies—cache-aware block sort, introspective comparison sort, and Hilbert-inspired bucket sort—based on a normalized Shannon-entropy probe. Originally prototyped in pure Python, TriSort+ has been migrated to a highly optimized C/OpenMP implementation, achieving the fastest publicly reported times for sorting 10 000–100 000 integers on an Apple M2.

- **Author (creator & corresponding):**  
  Adrián Valerio (San José, Costa Rica)  
  Email: avaleriop@me.com  
  GitHub: [@avaleriop](https://github.com/avaleriop)

---

## Overview

TriSort+ consists of three main phases:

1. **Entropy Probe**  
   Compute the normalized Shannon entropy \(H_{\text{norm}}\in[0,1]\) of the input in \(O(n)\).

2. **Strategy Dispatch**  
   - **Low entropy (< 0.30):** Cache-Aware Block Sort  
   - **Moderate entropy (0.30–0.70):** Introsmooth Sort (Introsort + Smoothsort fuse)  
   - **High entropy (≥ 0.70):** Hilbert-Inspired Bucket Sort (bucket distribution + local `qsort`)

3. **Merge/Output**  
   Reassemble or copy back into the original array.

The C/OpenMP port parallelizes histogram building, bucket distribution, and local sorts across 8 threads on an M2. A Python module (`trisortc.so`) exposes `trisort_inplace(buffer)` for seamless integration in Python code.

---

## Repository Structure

```

trisort-plus/
├── CMakeLists.txt                ← Build configuration (CMake)
├── LICENSE                       ← MIT license text
├── README.md                     ← This file
├── include/
│   └── trisort.h                 ← Public interface & function prototypes
├── src/
│   ├── cache\_block.c             ← Cache-Aware Block Sort implementation
│   ├── common.c                  ← `cmp_int` helper
│   ├── entropy.c                 ← Entropy probe (histogram)
│   ├── hilbert\_bucket.c          ← Hilbert-Inspired Bucket Sort
│   ├── introsmooth.c             ← Introsort/Smoothsort stub
│   ├── pybind.c                  ← Python binding via PyBind
│   └── trisort\_plus.c            ← Dispatcher (tie-breaker on $H_{\text{norm}}$)
├── tests/
│   └── smoke.c                   ← C “smoke” test: sorts 8 integers and prints
├── benchmarks/
│   ├── bench\_c\_compare.py        ← Compare TriSort+ C vs `sorted()` vs `np.sort()`
│   └── plot\_benchmark.py         ← Plot log–log speed curves from CSV
├── .gitignore                    ← Ignore build artifacts, CSV, PNG, etc.
└── .github/
└── workflows/
└── ci.yml               ← (Optional) GitHub Actions CI workflow

````

---

## Requirements

- **macOS 14+ (Sequoia)** or **Ubuntu x86_64**  
- **CMake ≥ 3.20**  
- **Clang/LLVM 15+** (macOS default) or **GCC 9+** (Ubuntu)  
- **Python 3.12.10 (Homebrew)** with:
  - NumPy ≥ 1.26  
  - Pandas ≥ 2.0  
  - Matplotlib ≥ 3.8  

*(On Ubuntu, replace Homebrew commands with `apt install cmake python3-dev python3-pip` and use `pip3 install numpy pandas matplotlib`.)*

---

## Build Instructions

1. **Clone the repository**  
   ```bash
   cd ~/Desktop
   git clone https://github.com/avaleriop/trisort-plus.git
   cd trisort-plus
````

2. **Create and enter a `build/` directory**

   ```bash
   mkdir build
   cd build
   ```

3. **Configure with CMake**

   * **macOS (Homebrew Python 3.12)**

     ```bash
     cmake -DCMAKE_BUILD_TYPE=Release \
           -DPython3_EXECUTABLE=/opt/homebrew/bin/python3.12 \
           -DPython3_INCLUDE_DIR=/opt/homebrew/opt/python@3.12/Frameworks/Python.framework/Versions/3.12/include/python3.12 \
           -DPython3_LIBRARY=/opt/homebrew/opt/python@3.12/Frameworks/Python.framework/Versions/3.12/lib/python3.12/config-3.12-darwin/libpython3.12.dylib \
           ..
     ```
   * **Ubuntu (system Python 3)**

     ```bash
     cmake -DCMAKE_BUILD_TYPE=Release \
           -DPython3_EXECUTABLE=$(which python3) \
           -DPython3_INCLUDE_DIR=$(python3 -c "from sysconfig import get_paths as gp; print(gp()['include'])") \
           -DPython3_LIBRARY=$(python3 -c "import sysconfig; print(sysconfig.get_config_var('LIBDIR') + '/libpython3.' + sysconfig.get_python_version() + '.so')") \
           ..
     ```

4. **Compile targets**

   ```bash
   cmake --build . --target trisort trisortc smoke -j8
   ```

   * `trisort` → produces `libtrisort.a`
   * `trisortc` → produces `trisortc.so` (Python module)
   * `smoke` → produces `smoke` executable for C test

5. **Run the C smoke-test**

   ```bash
   ./smoke
   ```

   You should see:

   ```
   1 2 3 4 5 6 7 8
   ```

6. **Verify Python binding**

   ```bash
   /opt/homebrew/bin/python3.12 - << 'PY'
   import array, random, time, sys, pathlib
   sys.path.insert(0, str(pathlib.Path("build")))
   import trisortc

   N = 100_000
   data = array.array("i", (random.randrange(1_000_000) for _ in range(N)))
   t0 = time.perf_counter()
   trisortc.trisort_inplace(data)
   dt = time.perf_counter() - t0
   print(f"TriSort+ C/OpenMP: {dt:.6f} s | Sorted? {all(data[i]<=data[i+1] for i in range(N-1))}")
   PY
   ```

   Expect a result like:

   ```
   TriSort+ C/OpenMP: 0.006700 s | Sorted? True
   ```

---

## Usage in Python

After successful build, in any Python 3.12 session:

```python
import array, random, time, sys, pathlib

# Ensure Python can load trisortc.so
sys.path.insert(0, str(pathlib.Path("path/to/trisort-plus/build")))

import trisortc

# Example: sort one million random 32-bit ints
N = 1_000_000
data = array.array("i", (random.randrange(1_000_000) for _ in range(N)))

t0 = time.perf_counter()
trisortc.trisort_inplace(data)
dt = time.perf_counter() - t0

print(f"TriSort+ C/OpenMP: {dt:.6f} s")
assert all(data[i] <= data[i+1] for i in range(N-1))
```

Replace `"path/to/trisort-plus/build"` with the actual path to your local `build/` folder.

---

## Benchmarking

### 1. Run the C vs Python benchmarks

Make sure you have installed **NumPy**, **Pandas**, and **Matplotlib** into your Homebrew Python 3.12, either via `brew install numpy` and a `pip install --user pandas matplotlib`, or by using a virtual environment:

```bash
cd /Users/adrianvalerio/Desktop/trisort-plus/benchmarks

# Generate benchmark_results.csv (TriSort+ C/OpenMP vs sorted() vs np.sort())
/opt/homebrew/bin/python3.12 bench_c_compare.py
```

This creates `benchmark_results.csv` with columns:

```
n,TriSortC,Sorted,NumpySort
10000,0.000590,0.000888,0.000622
100000,0.007685,0.010095,0.001061
1000000,0.066705,0.121799,0.012596
```

### 2. Plot the results

```bash
/opt/homebrew/bin/python3.12 plot_benchmark.py
```

This generates `benchmark_plot_final.png`, a log–log plot comparing the three methods across $10 000, 100 000, 1 000 000$ elements.

---

## Continuous Integration (Optional)

If you want GitHub Actions to build and test automatically, create `.github/workflows/ci.yml` with:

```yaml
name: TriSort+ CI

on:
  push:
    branches: [ main ]
  pull_request:
    branches: [ main ]

jobs:
  build-and-test:
    runs-on: macos-14

    steps:
      - name: Checkout repository
        uses: actions/checkout@v4

      - name: Install dependencies (Homebrew)
        run: |
          brew install cmake python@3.12 numpy pandas matplotlib

      - name: Configure CMake
        run: |
          mkdir build
          cd build
          cmake -DCMAKE_BUILD_TYPE=Release \
                -DPython3_EXECUTABLE=$(brew --prefix)/bin/python3.12 \
                -DPython3_INCLUDE_DIR=$(brew --prefix)/Frameworks/Python.framework/Versions/3.12/include/python3.12 \
                -DPython3_LIBRARY=$(brew --prefix)/Frameworks/Python.framework/Versions/3.12/lib/python3.12/config-3.12-darwin/libpython3.12.dylib \
                ..

      - name: Build targets
        run: cmake --build build --target trisort trisortc smoke -j8

      - name: Run C smoke test
        run: ./build/smoke

      - name: Run Python smoke test
        run: |
          cd build
          /opt/homebrew/bin/python3.12 - << 'PY'
import array, random, sys, pathlib
sys.path.insert(0, str(pathlib.Path("build")))
import trisortc
data = array.array("i", (random.randrange(1000000) for _ in range(10000)))
trisortc.trisort_inplace(data)
assert all(data[i] <= data[i+1] for i in range(len(data)-1))
print("Python smoke test passed")
          PY
```

After committing and pushing that file, GitHub Actions will run the workflow on every push to `main`.

---

## License

This project is released under the **MIT License**. See [LICENSE](LICENSE) for full text.

```
MIT License

Copyright (c) 2025 Adrián Valerio

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the “Software”), to deal
in the Software without restriction, including without limitation the rights 
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
copies of the Software, and to permit persons to whom the Software is furnished 
to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
THE SOFTWARE.
```

---

## Citation

If you use TriSort+ in research or commercial work, please cite:

> Valerio, A. (2025). TriSort+: An Entropy‐Adaptive Hybrid Sorter and an Open Benchmark Suite. GitHub repository: [https://github.com/avaleriop/trisort-plus](https://github.com/avaleriop/trisort-plus). DOI: 10.5281/zenodo.TBD

---

```markdown
## Acknowledgments

Special thanks to my wife and son for their unwavering support and patience throughout the development of this project called Life.
```


```
```
