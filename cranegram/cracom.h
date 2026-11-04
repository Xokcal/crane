#ifndef cracom_h
#define cracom_h

#include <stdio.h>
#include <time.h>

/* crane error describe log*/
#define craCM_error(line,error) \
 printf("\n%s: %s: line:%d \033[31merror\033[0m: %s\n", __FILE__, __TIME__, (line), (error))

#define addchar(s,c) combine_tail_char((s),(c));

#define addstr(s,n) combine_tail((s), (n));

#define clearstr(s) delete_all((s));

#define starttime\
 clock_t start_time, end_time;\
 double time_used;\
 start_time = clock();

#define endtime\
 end_time = clock();\
 time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;\
 printf("[%s]\033[32m execution consumption: %f s \033[0m\n", __TIME__, time_used);


#define errorcode 0xff
#define successcode 0x66

#endif