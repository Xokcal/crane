#include <stdio.h>
#include <string.h>
#include "crafunc.h"

#define BUILDIN_FUNC_SIZE 1

char *buildinfunc[BUILDIN_FUNC_SIZE] = {"print"};

int craF_isbuildinfunc(char *s)
{
 for(int i=0;i<BUILDIN_FUNC_SIZE;i++){
  if(!strcmp(s, buildinfunc[i]))return 1;
 }
 return 0;
}