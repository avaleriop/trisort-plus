# TriSort+ C/OpenMP

**TriSort+** is an entropy-adaptive hybrid sorting algorithm that dynamically chooses between three different sorting strategies—cache-aware block sort, introspective comparison sort, and Hilbert-inspired bucket sort—based on a normalized Shannon-entropy probe. Originally prototyped in pure Python, TriSort+ has been migrated to a highly optimized C/OpenMP implementation, achieving the fastest publicly reported times for sorting 10 000–100 000 integers on an Apple M2.

**Author (creator & corresponding):**  
Adrián Valerio Porras (San José, Costa Rica)  
Email: avaleriop@me.com  
GitHub: [@avaleriop](https://github.com/avaleriop)

## Overview

TriSort+ consists of three main phases:

1. **Entropy Probe**  
   Compute the normalized Shannon entropy \(H_{\text{norm}} \in [0,1]\) of the input in \(O(n)\).

2. **Strategy Dispatch**  
   - **Low entropy (< 0.30):** Cache-Aware Block Sort  
   - **Moderate entropy (0.30–0.70):** Introsmooth Sort (Introsort + Smoothsort fusion)  
   - **High entropy (≥ 0.70):** Hilbert-Inspired Bucket Sort (bucket distribution + local `qsort`)

3. **Merge/Output**  
   Reassemble or copy back into the original array.

The C/OpenMP port parallelizes histogram building, bucket distribution, and local sorts across 8 threads on an M2. A Python module (`trisortc.so`) exposes `trisort_inplace(buffer)` for seamless integration in Python code.


## Requirements

- **macOS 14+ (Sequoia)** or **Ubuntu x86_64**  
- **CMake ≥ 3.20**  
- **Clang/LLVM 15+** (macOS default) or **GCC 9+** (Ubuntu)  
- **Python 3.12.10 (Homebrew)** with:
  - NumPy ≥ 1.26  
  - Pandas ≥ 2.0  
  - Matplotlib ≥ 3.8  

> On Ubuntu, replace Homebrew commands with:

> sudo apt install cmake python3-dev python3-pip
> pip3 install numpy pandas matplotlib

## Build Instructions

1. **Clone the repository**  
   cd ~/Desktop
   git clone https://github.com/avaleriop/trisort-plus.git
   cd trisort-plus

2. **Create and enter a build/ directory**  
  mkdir build
  cd build

3. **Configure with CMake**
   **macOS (Homebrew Python 3.12)**
   cmake -DCMAKE_BUILD_TYPE=Release \
      -DPython3_EXECUTABLE=/opt/homebrew/bin/python3.12 \
-DPython3_INCLUDE_DIR=/opt/homebrew/opt/python@3.12/Frameworks/Python.framework/Versions/3.12/include/python3.12 \
-DPython3_LIBRARY=/opt/homebrew/opt/python@3.12/Frameworks/Python.framework/Versions/3.12/lib/python3.12/config-3.12-darwin/libpython3.12.dylib \
      ..

  **Ubuntu (system Python 3)**

  cmake -DCMAKE_BUILD_TYPE=Release \
      -DPython3_EXECUTABLE=$(which python3) \
      -DPython3_INCLUDE_DIR=$(python3 -c "from sysconfig import get_paths as gp; print(gp()['include'])") \
      -DPython3_LIBRARY=$(python3 -c "import sysconfig; print(sysconfig.get_config_var('LIBDIR') + '/libpython3.' + sysconfig.get_python_version() + '.so')") \
      ..

4. **Compile targets**

  cmake --build . --target trisort trisortc smoke -j8

trisort → produces libtrisort.a

trisortc → produces trisortc.so (Python module)

smoke → produces smoke executable for C test


  5. **Run the C smoke-test**

  ./smoke

/opt/homebrew/bin/python3.12 - << 'PY'
import array, random, time, sys, pathlib
sys.path.insert(0, str(pathlib.Path("build")))
import trisortc

  6. **Verify Python binding**

N = 100_000
data = array.array("i", (random.randrange(1_000_000) for _ in range(N)))
t0 = time.perf_counter()
trisortc.trisort_inplace(data)
dt = time.perf_counter() - t0
print(f"TriSort+ C/OpenMP: {dt:.6f} s | Sorted? {all(data[i]<=data[i+1] for i in range(N-1))}")
PY

## Benchmarking

Run the C vs Python benchmarks
Make sure you have installed NumPy, Pandas, and Matplotlib into your Homebrew Python 3.12 (via brew install numpy and pip install --user pandas matplotlib, or by using a virtual environment):

cd /Users/adrianvalerio/Desktop/trisort-plus/benchmarks

Generate benchmark_results.csv (TriSort+ C/OpenMP vs sorted() vs np.sort())
/opt/homebrew/bin/python3.12 bench_c_compare.py
This creates benchmark_results.csv with columns:

n,TriSortC,Sorted,NumpySort
10000,0.000590,0.000888,0.000622
100000,0.007685,0.010095,0.001061
1000000,0.066705,0.121799,0.012596
Plot the results

/opt/homebrew/bin/python3.12 plot_benchmark.py
This generates benchmark_plot_final.png, a log–log plot comparing the three methods across [10 000, 100 000, 1 000 000] elements.

## License
This project is released under the MIT License. See LICENSE for full text.

**MIT License**

Copyright (c) 2025 Adrián Valerio Porras

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
Citation
If you use TriSort+ in research or commercial work, please cite:

Valerio, A. (2025). TriSort+: An Entropy-Adaptive Hybrid Sorter and an Open Benchmark Suite. GitHub repository: https://github.com/avaleriop/trisort-plus. DOI: 10.5281/zenodo.TBD

**Acknowledgments**
I want to express my deepest gratitude to my wife and son for their unwavering support and patience throughout the development of all our life’s accomplishments, including this project.
