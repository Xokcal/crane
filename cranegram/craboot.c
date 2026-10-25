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


#define CRANE_VERSION   "1.0.0"
#define CRANE_AUTHOR    "Xokcal"
#define CRANE_BUILD     __DATE__ " " __TIME__
#define craP_crane_logo \
"   ___ ___    _   _  _ ___ \n" \
"  / __| _ \\  /_\\ | \\| | __|\n" \
" | (__|   / / _ \\| .` | _| \n" \
"  \\___|_|_\\/_/ \\_\\_|\\_|___|\n"

void print_welcome(void) {
    printf("\033[36m%s\033[0m\n", craP_crane_logo);
    printf("\033[33mCrane Compiler & VM\033[0m\n");
    printf("Version: %s\n", CRANE_VERSION);
    printf("Build:   %s\n", CRANE_BUILD);
    printf("Author:  %s\n", CRANE_AUTHOR);
    printf("Platform: Windows x86_64 (GCC)\n");
    
    printf("\nWelcome to Crane interactive terminal.\n");
    printf("Type your code to execute, or input '\033[1mh\033[0m' for help, '\033[1mq\033[0m' to quit.\n\n");
}

int main()
{
  int parsecode;
  //print_welcome();
  XokMalloc *txm = create_XokMalloc();
  String *origin_str = open_file("Crane/test.cr");
  TOKENSB *tokens = tokens_parse(txm, origin_str);
  //#define tsprint
  #ifdef tsprint
  for(int i = 0; i < tokens->count ; i++){
      logc("token" , tokens->tokens[i]->str);
  }
  #endif
  XokMalloc *xm = create_XokMalloc();
  Hashmap *hmap = create_hashmap(); /* gloab variable*/
  desclist *dl = Cra_desclist_create(xm , 16); /* tree list */
  parsecode = craP_token_parse(xm , tokens , dl);  /*  AST main parse */
  if(parsecode == errorcode)
   goto executeError;
  vm *vm = craC_bytecode_parse(xm , dl);
  craV_execute(xm , vm);
  executeError:
  /* free setting struct */
  Cra_desclist_free(xm , dl);
  free_XokMalloc(xm);
  free_XokMalloc(txm);
  return 0;
}