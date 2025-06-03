#include <stdlib.h>
#include <string.h>
#include "trisort.h"

void cache_block_sort(int *data, size_t n, size_t block)
{
    if (n <= block*2) {               /* pequeño → qsort */
        qsort(data, n, sizeof(int), cmp_int);
        return;
    }
    size_t blocks = (n + block - 1) / block;
    for (size_t i = 0; i < blocks; ++i) {
        size_t start = i * block;
        size_t len   = (start + block <= n) ? block : n - start;
        qsort(data + start, len, sizeof(int), cmp_int);
    }
    /* merge lineal in-place (galloping) */
    int *tmp = malloc(n * sizeof *tmp);
    size_t i = 0, j = block, k = 0;
    while (i < block && j < n)
        tmp[k++] = (data[i] <= data[j]) ? data[i++] : data[j++];
    memcpy(tmp + k, data + i, (block - i) * sizeof(int));
    memcpy(tmp + k + (block - i), data + j, (n - j) * sizeof(int));
    memcpy(data, tmp, n * sizeof(int));
    free(tmp);
}
