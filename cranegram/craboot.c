#include <stdio.h>
#include <ctype.h>
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

int main()
{
 setvbuf(stdout, NULL, _IOFBF, 1 << 20);
 /* start time */
 clock_t start_time, end_time;
 double time_used;
 // 1. 计时开始
 start_time = clock();
 /* ******** */
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

 clock_t start_timev, end_timev;
 double time_usedv;
 // 1. 计时开始
 start_timev = clock();
 craV_execute(xm, vmState);
 /* end time */
 end_timev = clock();
 // 3. 计算耗时（秒）
 time_usedv = ((double)(end_timev - start_timev)) / CLOCKS_PER_SEC;
 // 4. 打印结果
 printf("\n[%s][crane vm] execution consumption: %f s\n", __TIME__, time_usedv);
 executeError:
 /* free setting struct */
 craP_desclist_free(xm , dl);
 free_XokMalloc(xm);
 free_XokMalloc(txm);
 /* end time */
 end_time = clock();
 // 3. 计算耗时（秒）
 time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
 // 4. 打印结果
 printf("[%s][crane] execution consumption: %f s\n", __TIME__, time_used);
 return 0;
}
