#include <stdlib.h>
#include <math.h>
#include "trisort.h"

typedef struct { int key; int cnt; int used; } Bucket;

double entropy_norm(const int *data, size_t n)
{
    if (n == 0) return 0.0;

    size_t m = 1;                     /* next power of two ≥ 1.3 n */
    while (m < n * 1.3) m <<= 1;
    Bucket *tbl = calloc(m, sizeof *tbl);

    /* Insert */
    for (size_t i = 0; i < n; ++i) {
        size_t h = ((unsigned)data[i]) & (m - 1);    /* fast modulo */
        while (tbl[h].used && tbl[h].key != data[i])
            h = (h + 1) & (m - 1);
        if (tbl[h].used) tbl[h].cnt++;
        else { tbl[h].used = 1; tbl[h].key = data[i]; tbl[h].cnt = 1; }
    }

    double H = 0.0, invN = 1.0 / n;
    size_t k = 0;
    for (size_t i = 0; i < m; ++i) if (tbl[i].used) {
        k++;
        double p = tbl[i].cnt * invN;
        H -= p * log2(p);
    }
    free(tbl);
    return k ? H / log2((double)k) : 0.0;
}
