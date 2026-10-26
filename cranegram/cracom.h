#ifndef cracom_h
#define cracom_h

#include <stdio.h>

/* crane error describe log*/
#define craCM_error(error) \
 printf("%s:%s:\033[31merror\033[0m: %s\n", __FILE__, __TIME__, (error))

#define errorcode 0xff
#define successcode 0x66

#endif