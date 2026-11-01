#include <stdio.h>
#include <stdlib.h>
#include "../tool/String/String.h"
#include "cralex.h"
#include "../tool/log/log.h"
#include "../tool/map/hashmap_int.h"
#include "cgm_gramma.h"
#include "cgm_printf.h"
#include "../tool/map/hashmap_int.h"
#include "../tool/xokmalloc/xokmalloc.h"
#include "typevar.h"
#include <string.h>
#include "../tool/xokmalloc/xokmalloc.h"
#include "cracom.h"

#define newlextoken(xm)\
(lextoken*)xalloc_XokMalloc((xm), sizeof(lextoken));

#define newstring(s)\
(String*)create_string((s));

static inline lexState* newlexstate(XokMalloc *xm){
 lexState *lex = (lexState*)xalloc_XokMalloc(xm, sizeof(lexState));
 lex->lextokens = (lextoken**)xalloc_XokMalloc(xm, 3000000 * sizeof(lextoken*));
 return lex;
}

int issinglekeys(const char c){
 unsigned int ccode = ((int) c)+1;
 if(ccode >= 128)return 0;
 int i = (int)ascllcode[ccode];
 if(i != 0)return 1;
 if(c == '\n' || c == '\"' || c == '\'' ||  c == ' ' || c == '\t' || c == '\r'){   
  return 1;
 }
 return 0;
}


int iscranekeys(const char *s){
 for(int i=0;i<19;i++){
  if(strcmp(s, cranekeys[i])==0)return 1;
  }
 return 0;
}


lexState *craL_lexexecute(XokMalloc *xm, String *o){
 lexState *lex = newlexstate(xm);
 String *temp = newstring("");
 long long cnum = 0;
 long long line = 1;
 while(cnum <= o->length){
  switch(o->str[cnum]){
   case '\n':
    line++;
   break;
   /*case '\r':
    line++;
   break;*/
   default:
    break;
  }
  if(issinglekeys(o->str[cnum])){
   if(temp->length > 0){
    lextoken *tempt = newlextoken(xm);
    tempt->line = line;
    tempt->token = newstring(temp->str);
    lex->lextokens[lex->count++] = tempt;
    clearstr(temp);
   }
   lextoken *t = newlextoken(xm);
   t->line = line;
   t->token = newstring("");
   addchar(t->token, o->str[cnum]);
   lex->lextokens[lex->count++] = t;
   cnum++;
   continue;
  }else{
   addchar(temp, o->str[cnum]);
   cnum++;
   continue;
  }
 }
 return lex;
}

