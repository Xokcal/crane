#ifndef cracom_h
#define cracom_h

#include <stdio.h>

/* crane error describe log*/
#define craC_error(error) \
 printf("%s:%s:\033[31merror\033[0m: %s", __FILE__, __FILE_NAME__, error)

#define errorcode 0xff
#define successcode 0x66

#endif