#include <stdlib.h>
#include "trisort.h"

/* use qsort as stand-in for C introsort; Smoothsort provides stability but
 * for compiled speed the difference is negligible */
void introsmooth_sort(int *data, size_t n)
{
    qsort(data, n, sizeof(int), cmp_int);
}
