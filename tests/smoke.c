#include <stdio.h>
#include "trisort.h"

int main(void) {
int arr[8] = {7,2,5,3,1,8,4,6};
trisort_plus_c(arr, 8);
for (int i = 0; i < 8; i++) {
printf("%d ", arr[i]);
}
printf("\n");
return 0;
}
