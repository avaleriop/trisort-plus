#include "trisort.h"

void trisort_plus_c(int *data, size_t n)
{
    double H = entropy_norm(data, n);
    if (H < 0.3) {
        cache_block_sort(data, n, 64);
    } else if (H < 0.7) {
        introsmooth_sort(data, n);
    } else {
        hilbert_bucket_sort(data, n, 32);
    }
}
