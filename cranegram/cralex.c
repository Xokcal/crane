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
 for(int i=0;i<31;i++){
  if(c==signkeys[i])return 1;
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
   case '\r':
    line++;
   break;
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
 logi("all count", lex->count);
 return lex;
}


/*lexState *
craL_lexexecute(XokMalloc *txm, String *origin)
{
 String **tokens = (String**)xalloc_XokMalloc(txm, 2000000 * sizeof(String*));
 String *temp = create_string("");
 String *temp_test = create_string("");
 String *temp_longest_match = create_string("");
 String *extre_check_key = create_string("");
 int count = 0;
 starttime;
 for (int i = 0; i < origin->length; ++i) {
  //LOG_CHR("---i---" , origin->str[i]);
  if(isExtreCheckChar(origin->str[i])){
   combine_tail_char(extre_check_key , origin->str[i]);
   combine_tail_char(extre_check_key , origin->str[i + 1]);
   if(isPlaceHoldKeys(extre_check_key)){
    tokens[count] = create_string("");
    combine_tail(tokens[count++] , temp->str);
    delete_all(temp);
    tokens[count] = create_string("");
    combine_tail(tokens[count++] , extre_check_key->str);
    delete_all(extre_check_key);
    i += 1;
    continue;
   }
  }
  combine_tail_char(temp , origin->str[i]);
  if(isContainKeys(temp) == 1){
   int longest_count = 1;
   combine_tail(temp_longest_match , temp->str);
   if(origin->str[i + 1] == ' ' ||  (i + 1) == origin->length){
    goto notAddBehind;
   }
   combine_tail_char(temp_longest_match , origin->str[i + longest_count]);
   //logc("[1]" , temp_longest_match->str);
   if(isContainKeys(temp_longest_match)){
    //log("KEYS");
    while(!(origin->str[i + (++longest_count)] == ' ') || (1 + longest_count) != origin->length){
     combine_tail_char(temp_longest_match , origin->str[i + longest_count]);
     if(!isContainKeys(temp_longest_match)){
      delete_indexof(temp_longest_match , temp_longest_match->length - 1);
      break;
     }
    }
    tokens[count] = create_string("");
    //logc("---collect---/longest" , temp_longest_match->str);
    combine_tail(tokens[count++] , temp_longest_match->str);
    delete_all(temp);
    delete_all(temp_longest_match);
    i = i + 1;
    continue;
   }
   notAddBehind:
   tokens[count] = create_string("");
   //logc("---collect---/keys" , temp->str);
   combine_tail(tokens[count++] , temp->str);
   delete_all(temp);
   delete_all(temp_longest_match);
   continue;
  }else{
   combine_tail_char(temp_test , origin->str[i + 1]);
   if(isContainKeys(temp_test)){
    tokens[count] = create_string("");
    //logc("---collect---/common" , temp->str);
    combine_tail(tokens[count++] , temp->str);
    delete_all(temp);
   }
   delete_all(temp_test);
   continue;
  }
 }
 endtime;
 //printf("[count]%d\n" , count);
 string_free(temp);
 string_free(temp_test);
 string_free(temp_longest_match);
 string_free(extre_check_key);
 TOKENSB *tokensb = (TOKENSB*)xalloc_XokMalloc(txm, sizeof(TOKENSB));
 tokensb->tokens = tokens;
 tokensb->count = count;
 return tokensb;
}*/


