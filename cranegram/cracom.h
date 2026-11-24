#ifndef cracom_h
#define cracom_h

#include <stdio.h>
#include <time.h>

#define P_DEFUALT "\033[0m"
#define P_RED
#define P_GREEN "\033[32m"
#define P_BLUE_GREEN "\033[36m"
#define P_PINK "\033[35m"

/* crane error describe log*/
#define craCM_error(line,error, text) \
 printf("\n" P_BLUE_GREEN "%s\033[0m: %s | %s\n %d |" P_PINK "   error\033[0m: %s\n", __FILE__, __TIME__, (text),(line), (error))

#define craCM_show(show) \
 printf("[crane origin code show]:\n\033[36m%s\033[0m\n", (show));


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