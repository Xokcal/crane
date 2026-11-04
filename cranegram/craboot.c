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


int main(){
 starttime;
 setvbuf(stdout, NULL, _IOFBF, 1 << 20);
 int parsecode;
 XokMalloc *txm = create_XokMalloc();
 String *context = open_file("test/grm.cr");
 //logc("context",context->str);
 lexState *lexstate = craL_lexexecute(txm, context);
 //#define tsprint
 #ifdef tsprint
 for(int i = 0; i < lexstate->count ; i++){
  logc("token" , lexstate->lextokens[i]->token->str);
 }
 #endif
 XokMalloc *xm = create_XokMalloc();
 Hashmap *hmap = create_hashmap(); /* gloab variable*/
 desclist *dl = craP_desclist_create(xm , 1); /* tree list */
 parsecode = craP_parsedesc(xm , lexstate , dl);  /*  AST main parse */
 if(parsecode == errorcode)
  goto executeError;
 vmState *vmState = craC_bytecode_parse(xm , dl);
 craV_execute(xm, vmState);
 executeError:
 /* free setting struct */
 craP_desclist_free(xm , dl);
 free_XokMalloc(xm);
 free_XokMalloc(txm);
 endtime;
 return 0;
}

