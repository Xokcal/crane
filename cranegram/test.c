#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../tool/String/String.h"   // 只包含声明

static double now_sec (void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

int m (void) {
    String *s = create_string("");
    double t0 = now_sec();
    for (int i = 0; i < 10000; i++)
        combine_tail_char(s, 'x');
    double t1 = now_sec();
    printf("combine_tail_char 10000 time: %.6f s, len=%zu, size=%zu\n",
           t1 - t0, s->length, s->size);
    string_free(s);

    s = create_string("");
    double t2 = now_sec();
    for (int i = 0; i < 100000; i++)
        combine_tail_char(s, 'x');
    double t3 = now_sec();
    printf("combine_tail_char 100000 time: %.6f s, len=%zu, size=%zu\n",
           t3 - t2, s->length, s->size);
    string_free(s);

    return 0;
}