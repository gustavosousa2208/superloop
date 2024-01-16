#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int * arg, char ** argc) {
    struct timespec start, end;
    int i = 0;
    int j = 0;
    int k = 0;
    int l = 0;
    int m = 0;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    // print start time with seconds and nano seconds
    printf("start time: %ld s, %ld ns\n", start.tv_sec, start.tv_nsec);

    return 0;
}