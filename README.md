# TriSort+ C/OpenMP

**TriSort+** is an entropy-adaptive hybrid sorting algorithm that dynamically chooses between three different sorting strategies—cache-aware block sort, introspective comparison sort, and Hilbert-inspired bucket sort—based on a normalized Shannon-entropy probe. Originally prototyped in pure Python, TriSort+ has been migrated to a highly optimized C/OpenMP implementation, achieving the fastest publicly reported times for sorting 10 000–100 000 integers on an Apple M2.  

- **Author (creator & corresponding):**  
  Adrián Valerio (San José, Costa Rica)  
  Email: avaleriop@me.com  
  GitHub: [@avaleriop](https://github.com/avaleriop)  

—  
## Overview

TriSort+ consists of three main phases:

1. **Entropy Probe**  
   Compute the normalized Shannon entropy \(H_{\text{norm}}\in[0,1]\) of the input in \(O(n)\).

2. **Strategy Dispatch**  
   - **Low entropy (< 0.30):** Cache-Aware Block Sort (minimize comparisons).  
   - **Moderate entropy (0.30–0.70):** Introsmooth Sort (Introsort + Smoothsort fusion).  
   - **High entropy (≥ 0.70):** Hilbert-Inspired Bucket Sort (distribute + local `qsort`).  

3. **Merge/Output**  
   Reassemble or copy back into the original array.  

The C/OpenMP port parallelizes histogram building, bucket distribution, and local sorts across 8 threads on an M2. A Python module (`trisortc.so`) exposes `trisort_inplace(buffer)` for seamless integration in Python code.

—  
## Repository Structure

