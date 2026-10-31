#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "craparse.h"
#include "../tool/xokmalloc/xokmalloc.h"
#include "cralex.h"
#include "cgm_gramma.h"
#include "../tool/String/String.h"
#include "../tool/log/log.h"
#include "cgm_file.h"
#include "cracode.h"
#include "cravm.h"
#include "cracom.h"


#include <time.h>

#define starttime\
 clock_t start_time, end_time;\
 double time_used;\
 start_time = clock();

#define endtime\
 end_time = clock();\
 time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;\
 printf("[%s][] execution consumption: %f s\n", __TIME__, time_used);
 

int main()
{
 setvbuf(stdout, NULL, _IOFBF, 1 << 20);
 int parsecode;
 XokMalloc *txm = create_XokMalloc();
 String *origin_str = open_file("test/test.cr");
 TOKENSB *tokens = craL_lexexecute(txm, origin_str);
 //#define tsprint
 #ifdef tsprint
 for(int i = 0; i < tokens->count ; i++){
  logc("token" , tokens->tokens[i]->str);
 }
 #endif
 XokMalloc *xm = create_XokMalloc();
 Hashmap *hmap = create_hashmap(); /* gloab variable*/
 desclist *dl = craP_desclist_create(xm , 1); /* tree list */
 parsecode = craP_parsedesc(xm , tokens , dl);  /*  AST main parse */
 if(parsecode == errorcode)
  goto executeError;
 vmState *vmState = craC_bytecode_parse(xm , dl);
 craV_execute(xm, vmState);
 executeError:
 /* free setting struct */
 craP_desclist_free(xm , dl);
 free_XokMalloc(xm);
 free_XokMalloc(txm);
 return 0;
}
