/*  hilbert_bucket.c  —  TriSort+ bucket sort inspirado en curva de Hilbert
 *
 *  Estrategia:
 *    1.  Encuentra mínimo y máximo en O(n).
 *    2.  Calcula índice de bucket por proyección lineal:
 *           idx = ((x - min) * (B-1)) / (max - min + 1)
 *        (la constante +1 evita división por cero).
 *    3.  Cuenta cuántos elementos caerán en cada bucket (primer paseo).
 *    4.  Construye un array prefix-sum  → offset inicial de cada bucket.
 *    5.  Distribuye en un buffer temporal (segundo paseo).
 *    6.  Ejecuta `qsort()` por bucket; los buckets pequeños caben en cache.
 *    7.  Copia el buffer ordenado de vuelta al arreglo original.
 *
 *  Complejidad:
 *      Tiempo  O(n)  +  Σ  m_i·log(m_i)   (m_i = tamaño bucket i)
 *      Memoria O(n)  (buffer temporal)  +  O(B)  (contadores / offsets)
 *
 *  Adecuado para H_norm ≥ 0.7 donde el número de valores distintos es alto.
 */

#include <stdlib.h>
#include <string.h>
#include "trisort.h"

/* comparador para qsort */
int cmp_int(const void *a, const void *b)
{
    return (*(const int *)a - *(const int *)b);
}

void hilbert_bucket_sort(int *data, size_t n, int B)
{
    if (n < 64 || B < 2) {                 /* problema demasiado pequeño   */
        qsort(data, n, sizeof(int), cmp_int);
        return;
    }

    /* 1. encontrar rango */
    int min = data[0], max = data[0];
    for (size_t i = 1; i < n; ++i) {
        if (data[i] < min) min = data[i];
        else if (data[i] > max) max = data[i];
    }
    if (min == max) return;                /* todos iguales, ya ordenado   */

    /* 2. reservar contadores y buffers auxiliares */
    size_t *cnt = calloc(B, sizeof *cnt);
    size_t *ofs = malloc((B + 1) * sizeof *ofs);
    int    *buf = malloc(n * sizeof *buf);

    /* 3. primer paseo: contar elementos por bucket */
    const double scale = (double)(B - 1) / (double)(max - min + 1);
    for (size_t i = 0; i < n; ++i) {
        int idx = (int)((data[i] - min) * scale);
        cnt[idx]++;
    }

    /* 4. prefix-sum para offsets in-place             ofs[0] = 0       */
    ofs[0] = 0;
    for (int b = 0; b < B; ++b) ofs[b + 1] = ofs[b] + cnt[b];

    /* copia de offsets para avance */
    size_t *next = calloc(B, sizeof *next);
    memcpy(next, ofs, B * sizeof *next);

    /* 5. segundo paseo: distribuir a buf */
    for (size_t i = 0; i < n; ++i) {
        int idx = (int)((data[i] - min) * scale);
        buf[next[idx]++] = data[i];
    }

    /* 6. ordenar cada bucket localmente */
    for (int b = 0; b < B; ++b) {
        size_t start = ofs[b];
        size_t len   = cnt[b];
        if (len > 1) qsort(buf + start, len, sizeof(int), cmp_int);
    }

    /* 7. copiar de vuelta */
    memcpy(data, buf, n * sizeof *buf);

    free(buf);
    free(cnt);
    free(ofs);
    free(next);
}
