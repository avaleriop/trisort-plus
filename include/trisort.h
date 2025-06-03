#ifndef TRISORT_H
#define TRISORT_H
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

double entropy_norm(const int *data, size_t n);
void hilbert_bucket_sort(int *data, size_t n, int buckets);
void cache_block_sort(int *data, size_t n, size_t block);
void introsmooth_sort(int *data, size_t n);          /* Introsort + Smoothsort fuse */
void trisort_plus_c(int *data, size_t n);

/* Comparator exported for tests / qsort fallback */
int cmp_int(const void *a, const void *b);

#ifdef __cplusplus
}
#endif
#endif /* TRISORT_H */
