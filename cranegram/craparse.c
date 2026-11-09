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
#include "types.h"

/*
** judge current token char* whither is high priority 
** sign or low priority sign.
*/
static int ishighprioritysign(char *n){
 for(int i = 0 ;i  <HIGH_PRI_SIGN_SIZE; i++){
  if(strcmp(n, highprisign[i])==0)return 1;
 }
 return 0;
}


static int islowprioritysign(char *n){
 for(int i = 0; i < LOW_PRI_SIGN_SIZE; i++){
  if(strcmp(n, lowprisign[i])==0)return 1;
 }
 return 0;
}


static int iscondisign(char *n){
 for(int i = 0; i < CONDI_SIGN_SIZE; i++){
  if(strcmp(n, condisign[i])==0)return 1;
 }
 return 0;
}


/*
** find has store to desclist assign craastdesc by variable name. 
*/
static craastdesc *findvardesc(desclist *dl, char *name){
 for(int i = 0; i < dl->count; i++){
  if(!strcmp(dl->desclist[i]->as.assign.name, name)){
    return dl->desclist[i];
  }
 }
 return NULL;
}


/*
** token convert enum asttype, return to tree parse switch. 
*/
static int tokentype(String *s){
 if(compare(s, "int")||compare(s, "float")
 ||compare(s, "double")||compare(s, "char")
 ||compare(s, "long"))return GRAM_VAR;
 if(compare(s , " "))return GRAM_EMPTY;
 if(compare(s , "\n"))return GRAM_EMPTY;
 if(compare(s , "\r"))return GRAM_EMPTY;
 if(compare(s , ";"))return GRAM_EMPTY;
 if(compare(s , "print"))return GRAM_UNKEY;
 if(compare(s , "-"))return GRAM_NOTE;
 if(compare(s , "if"))return GRAM_IF;
 if(compare(s , "}"))return GRAM_BLOCK_END;
 if(!iscranekeys(s->str))return GRAM_UNKEY;
 return GRAM_EMPTY;
}


/*
** judge string whither is number: isnumber return 1, nonumber return 0.
*/
static int isnumber(const char *str){
 if (str == NULL || *str == '\0') return 0;
 while (*str != '\0') {
  if (!isdigit(*str)) return 0;
  str++;
 }
 return 1;
}


static inline int accept(ParseFactor *pf , char *name){
 if(compare(pf->tokens->lextokens[pf->curr]->token , name)){
   pf->curr++;
   return 1;
 }
 return 0;
}


int cmpkey(char *s){return 0;}


static void accept_empty(ParseFactor *pf){
 while(pf->curr < pf->tokens->count)
 {
   String *s = pf->tokens->lextokens[pf->curr]->token;
   if(compare(s , " "))pf->curr++;
   else return;
 }
}


/* 
** test whither have next like: {args, assign, ..}
*/
static inline int testnext(ParseFactor *pf , char *s){
 accept_empty(pf); /* check current pf not is empty key */
 int bl = strcmp(pfcurstr(pf), s) == 0 ? 1 : 0;
 pfnext(pf);
 return bl;
}


/* 
** packgae xm, parsefactor, root to assign number tree tackle. 
*/
static inline pavstate pavstate_init(XokMalloc *xm,ParseFactor *pf){
 pavstate pavs; pavs.xm = xm;
 pavs.pf = pf; pavs.root = NULL;
 return pavs;
}


/*
** check ast assign tree node detailed data and value.
*/
static void print_ast(craastdesc *node, int depth){
 if (!node) {
  printf("%*s(null)\n", depth * 2, "");
  return;
 }
 printf("%*s", depth * 2, "");
 switch (node->kind) {
 case AST_NUM:
  printf("NUM: %d\n", node->as.num);
  break;
 case AST_LVNAME:
  printf("LVNAME: %s\n", node->as.lvname);
 case AST_VAR:
  printf("VAR: %s\n", node->as.varname);
  break;
 case AST_BINOP:
  printf("BINOP: %s\n", node->as.binop.calcsign);
  print_ast(node->as.binop.left,  depth + 1);
  print_ast(node->as.binop.right, depth + 1);
  break;
 case AST_ASSIGN:
  printf("ASSIGN: %s\n", node->as.assign.name);
  print_ast(node->as.assign.value, depth + 1);
  break;
 default:
  printf("UNKNOWN kind=%d\n", node->kind);
  break;
 }
}


/*
** Collect target line origin code string by loop.
** Common execute this function when programme happen erro or bug.
*/
static String *linecontext(ParseFactor *pf, long line){
 String *t = create_string("");
 for(int i = 0; i < pf->tokens->count; i++){
  if(pf->tokens->lextokens[i]->line == line){addstr(t, pfcurstr(pf));}
  else if(pf->tokens->lextokens[i]->line > line)return t;  /* Current line 
  number > target line is return. */
 }
 return t;
}


/*
** Variable is undefine status, parse to not have assign value's expdesc.
*/
static inline craastdesc* undefvardesc(XokMalloc *xm 
                     , String *name , String *type){
return make_assign(xm , name->str , NULL , type->str);
}


/*
** Assign value expdesc node parse factor, assign value's number, function, 
** arrays index value,... parse to desc tree and return mid high priority
** and low priority function tackle.
*/
static craastdesc* parse_factor(pavstate ps){
 /* tackle curr number to struct 'craastdesc*' */
 if(isnumber(craP_ps_currtoken(ps)->str)){
  int *n = str_convert_int(craP_ps_currtoken(ps));
  pfnext(ps.pf);  /* throught number string */
  /* ps.root is only result accept desc, and return ps.root */
  ps.root = make_num(ps.xm , *n);
  free(n);  /* n is malloc variable */
 }else{  /* not is number condition */
  ps.root = make_lvalue(ps.xm, craP_ps_currtoken(ps)->str);
  pfnext(ps.pf);
 }
 /* pf's index step to next unempty token */
 accept_empty(ps.pf); /* to next unempty token */
 return ps.root;
}



static craastdesc* parse_mid(pavstate ps){
 accept_empty(ps.pf);
 /* Low priority calculate sign ,tackle struct 'craastdesc*' */
 craastdesc *l = parse_factor(ps); /* is 'main node' in the funtion */
 String *cur = craP_ps_currtoken(ps);
 /* continuous tackle high priority condition */
 while(ishighprioritysign(cur->str)){
  pfnext(ps.pf);
  accept_empty(ps.pf); /* Current pf.token is calcsign */
  /* Ps entry parse_factor ps root address convert, but previous
  ps root's desc result address is store to craastdesc *l */
  craastdesc *r = parse_factor(ps);
  l = make_binop(ps.xm , cur->str , l , r);
  /* Every loop, l is after refresh l and loop internal r(num) 
  make_binop() overall root node */
  cur = craP_ps_currtoken(ps); /* Refresh current calcsign */
 }
 /* External funtion need r node, l is has tackled all continuous high priority
 node tree, and/or first entre the funtion's low priority node(tree) */
 return l;
}


static craastdesc* parsevaluemain(pavstate ps){
 accept_empty(ps.pf);
 /* Tackle low priority or continous tackle high priority,
  use ps.root to accept res , root is current all craastdesc node */
 ps.root = parse_mid(ps); 
 /* Drawer parse_mid() , current caculate sign */
 String *cur = craP_ps_currtoken(ps);
 while (islowprioritysign(cur->str)){
  pfnext(ps.pf);
  accept_empty(ps.pf); /* Current is match right number */
  craastdesc *r = parse_mid(ps);
  ps.root = make_binop(ps.xm , cur->str , ps.root , r);
  /* Ps.root is previous ps.root and craastdesc *r binop node 
  (about parse_mid() internal high priority root accept overall node),
  and current pf.token is calcsign */
  cur = craP_ps_currtoken(ps); /* Refresh current calcsign */
 }
 /* Root is main node, has collected all tackled node tree */
 return ps.root; 
}


/*
** Variable parse to expdesc main function.
** About differ variable type, and differ variable garmmar express type.
*/
static craastdesc* vardesc(XokMalloc *xm, ParseFactor *pf, String *t){
 String *type = create_string(t->str);
 accept(pf , acptkint);  /* Int type var */
 accept_empty(pf);  /* Throught empty to variable name */
 String *name = craP_pf_currtoken(pf);
 pfnext(pf);
 craastdesc* var = make_var(xm , name->str);  /* Variable's name */
 accept_empty(pf);  /* Throught empty to '=' */
 int isequ = accept(pf , "=");  /* Judge '=' */
 if(!isequ){  /* If isEqu not's true , declare is 'int a;' */
  craastdesc *undefinevar = 
  undefvardesc(xm , name , type);  /* Undefine variable tackle */
  return undefinevar;
 }
 pavstate ps = pavstate_init(xm , pf);
 craastdesc *v = parsevaluemain(ps); /* Tackle after '=' value tree */
 /* Current 'desc' struction is: struct assign* = {var_name , struct binop* root} */
 craastdesc *desc = make_assign(
 xm , name->str , v , type->str);
 return desc;
}



/*
** Function str param type, record like: {"hello crane!"}
** originstr = "hello crane!", and tackle print in virtual machine
** package by struct Tparam.
*/
static void stringargs(ParseFactor *pf, craastdesc *desc, String *t){
 if(strcmp(pfcurstr(pf) , "\"")==0){ /* Is string */
  accept(pf , "\""); /* eat '|"' */
  while(strcmp(pfcurstr(pf) , "\"") != 0){
   cmptail(t, pfcurstr(pf)); /* Main content */
   pfnext(pf);
  }/* Current pf.token is double parenthesee,
  need slip the token by pfnext() later */
  Tparam tp;
  tp.pty = PRM_STRING;
  tp.originstr = strdup(t->str);
  /* Tparam tp push desc func args, and selfadd argsi. */
  callpushargs(desc, tp);  /* Tparam assign
  to outed arrays is forever store */
  strclear(t);  /* Template String* delete all content. */
  pfnext(pf);  /* Slip '\"'. */
  accept_empty(pf);  /* Current chould is ',' or ')'  */ 
 }
}


/* 
** Could is ',' or crane key if the funtion have more args,
** and string args not is last args like print(a , "crane" , 13).
**           ',' (be thought it was variable args).   ____/
*/
static void singletokenargs(ParseFactor *pf, craastdesc *desc){
  /*Function issinglekeys() args is char type 
 and pfcurstr() result is char*, so judge whither 
 is ',' or singlesign use result[0] first char. */
 if(issinglekeys(pfcurstr(pf)[0]))return; 
 Tparam tp;
 /* single string param about 'number', 'variable', 'enum', ..
 judge store param type by condition judgement. */
 if(craC_isstrvar(pfcurstr(pf)))
 tp.pty = PRM_VAR;
 else if(isnumber(pfcurstr(pf)))
 tp.pty = PRM_NUMBER;
/*  Store origin string, convert number type(int, long, float, double, 
 ..) in virtual machine. */
 tp.originstr = strdup(pfcurstr(pf));
 callpushargs(desc, tp);
 pfnext(pf);
 accept_empty(pf);
}


/*
** Unkey use to call(function use call) main execute.
*/
static int unkeycallexe(ParseFactor *pf, craastdesc *desc
                           , String *t, char *unkeytype){
 if(!strcmp(unkeytype , "(")){  /* "(" is function! */
  pfnext(pf);
  do{if(!strcmp(pfcurstr(pf) , ")"))
   /* If function not have args like: "print();"
   , next token is ')' can return. */
   return successcode;
   /* Current is '(' next token */
   accept_empty(pf);  /* confirm not is empty, reach first param */
   stringargs(pf , desc , t);
   singletokenargs(pf , desc);
  }while(testnext(pf, ","));  /* If have one or more args, at least 
  loop one time, have ',' execute next args and selfadd pf.curr. */
  if(accept(pf , ";"))
   return successcode;
  craCM_error(craC_tokencurline(pf),
   "function not have end: \";\" ");
  return errorcode;
 }
}


/*
** Unkey tackle call execute, string parse to craastdesc call.
*/
static int unkeycallexemain(XokMalloc *xm, ParseFactor *pf,
               craastdesc **desc, char *unkey, String *t){
 int unkeycode; /* accept unkey_desc return code*/
 if(!strcmp(pfcurstr(pf) , "(")){
  *desc = make_call(xm, unkey);
  /* store, use to judge unkey how about grammar type*/
  char *unkeytype = strdup(pfcurstr(pf));
  /* priority judge function */
  unkeycode = unkeycallexe(pf, *desc, t, unkeytype);
  free(unkeytype);  /* free unkey type */
  free(unkey);  /* free unkey name */
  if(unkeycode == errorcode)return errorcode;
  return successcode;
 }
}


/*
** Unkey tackle hasvar execute, if is '=' judge is hasvar.
** Assume function unkey_desc def desc self address is 0X001, craastdesc**desc accept
** out desc address and assign to out desc address's value is new desc(make_hasvar).
*/
static void unkeyhasvarexe(XokMalloc *xm, ParseFactor *pf,
            desclist *dl, craastdesc **desc, char *unkey){
 if(!strcmp(pfcurstr(pf), "=")){
  accept(pf, "=");  /* current is function unkey_desc entry by
  judge after unkey token, authentic content is next. */
  pavstate ps = pavstate_init(xm , pf);
  /* Parse reassign binop after '='. */
  craastdesc *v = parsevaluemain(ps);
  craastdesc *target = findvardesc(dl, unkey);
  if(target==NULL)return;
  *desc = make_hasvar(
  xm, v, unkey, target->as.assign.type);
  free(unkey);
 }
}


/* 
** Unkey about like function, selfadd, selfsub,..,the unkey need 
** special tackle, unkey not is key, and not is queto, parenthesee,
** .., so overall call these is'unkey'.
*/
static craastdesc* unkeydesc(XokMalloc *xm, 
            ParseFactor *pf, desclist *dl){
 pfback(pf);  /* Current pf is slip key,
 pf need back to previous index get varibale key */
 craastdesc *desc; /* Overall desc node */
 /* str tool, convenient to tackle str */
 String *t = create_string("");        
 char *unkey = strdup(pfcurstr(pf)); /* Store function name */
 pfnext(pf); /* Next is {'(', '++', '--', "+=", '-=', ...}, 
 judge unkey specific type  */
 accept_empty(pf);
 unkeyhasvarexe(xm, pf, dl, &desc, unkey);
 int code = unkeycallexemain(xm, pf, &desc, unkey, t);
 strfree(t); /* priroity free String, then return */
 if(code == errorcode)return NULL;
 return desc;
}


/*
** Crane grammar 'note' desc execute, '--' note only use to oneline,
** if is next line '--' note is uneffectived.
*/
static int notedesc(ParseFactor *pf){
  /* switch auto Slip first '-' judge next token whither is '-'. */
 if(!strcmp(pfcurstr(pf), "-")){
  /* From current pf tokenpoint to tokens end, if reach '\n' exit. */
  for(int i = pf->curr; i < pf->tokens->count; i++){  
   if(!strcmp(pfcurstr(pf), "\n"))return 0;  /* Current is token '\n'. */
   pfnext(pf);}
 }
 long line = getcurline(pf);  /* Record error line. */
 /* get target line origin code string */
 String *lineorigincode = linecontext(pf, line);
 linecontext(pf, line);
 /* Tell use error reason. */
 craCM_error(line, "grammar only have single '-' . ");
 /* Tell reatil error origin code content. */
 craCM_error(getcurline(pf), lineorigincode->str);
 strfree(lineorigincode);  /* Free String* t */
 return 1;
}


/*
** if(a < 10 or a2 > 1 and a3 + 9 eq 19)
** Tackle compare describtion tree execute, pf token start to first content. 
*/
static craastdesc *cmpdescexe(XokMalloc *xm, ParseFactor *pf){
 pavstate ps = pavstate_init(xm , pf);  /* Package xm, pf to ps */
 /* Token is compare char when parsevaluemain() return 
 and current token is first authentic contene */
 craastdesc *lv = parsevaluemain(ps);  
 char *cmpsign = pfcurstr(pf);
 pfnext(pf); accept_empty(pf);  /* Reach compare sign next value */
 craastdesc *rv = parsevaluemain(ps);
 craastdesc *cmpdesc = make_cmp(xm, cmpsign, lv, rv);
 return cmpdesc;  /* current token is condition sign */
}


/*
** If condition parse ast tree factor and mid combine function.
** Tackle high condition priority condition sign and continous tree create.
*/
static craastdesc *ifcondimid(XokMalloc *xm, ParseFactor *pf){
  /* Return cmpdescexe() compare tree result current is condition sign*/
 craastdesc *l = cmpdescexe(xm, pf);
 /* Reach 'and' condition sign continous
  collect high priority condition. */
 while(strcmp(pfcurstr(pf), "and")==0){
  char *condic = pfcurstr(pf);   /* Record current condition sign */
  pfnext(pf); accept_empty(pf);  /* Reach next authentic content */
  craastdesc *r = cmpdescexe(xm, pf);
  /* Up craastdesc l is single binop, here r is new 
  cmpdesc, r and l make new condidesc, l equal new 
  condition desc if have next while l is this new condition
   desc and continous combine new r cmpdesc. */
  l = make_condi(xm, condic, l, r);  /* Make condition tree */
 }
 return l;
}


/*
** if condition tree main execute function and condition 
** low priroity execute factor
*/
static craastdesc *ifcondimain(XokMalloc *xm, ParseFactor *pf){
 craastdesc *l = ifcondimid(xm, pf);  /* Desc l is main desc */
 /* In ifcondimid has continous combine 'and' 
 condition,  next only is 'or' and entry */
 while(strcmp(pfcurstr(pf), "or")==0){
  char *condic = pfcurstr(pf);  /* Store this condition sign */
  pfnext(pf); accept_empty(pf);
  /* Again use function ifcondimid(), judge next whitehr
   is 'and', if is.. continous combine and return to r*/
  craastdesc *r = ifcondimid(xm, pf);  
  l = make_condi(xm, condic, l, r);  /* Combine r and l, and 
  judge next condition sign whither is 'or', because condition 
  sign 'and' hav been combine by previous ifcondimid() */
 }
 return l;
}


/*
** If key main execute, by ifcondimain(), block()... function tackle to desc tree.
*/
static craastdesc *ifdesc(XokMalloc *xm, 
         ParseFactor *pf, lexState *ts){
 accept_empty(pf);  /* Ensure current token not is empty */
 accept(pf, "(");  /* Current token probably is '(' and eat */
 accept_empty(pf);  /* Current is authentic content */
 /* First execute if comdition tree */
 craastdesc *condi = ifcondimain(xm, pf);/* Parse if condition desc 
 tree, tokesn is "(" when entry, and out this function token is ")" */
 pfnext(pf); accept_empty(pf);
 /* Eat "{" judge current whither is this token */
 accept(pf, "{"); accept_empty(pf);
 /* Tblock use to record "{..}" inner content tree */
 Tblock *ifblock = newTblock(xm);
 /* Ifdesc use to record condition tree and {..} content*/
 craastdesc *ifdesc = make_ifdesc(xm, condi, ifblock);
 /* {..} content parse tree by use craP_parsedesc() */
 pf->curr = craP_parsedesc(xm, ts, ifblock->treelist, pf->curr);
 if(ifdesc->kind == AST_IF){log("is IF");}
 return ifdesc;
}


/*
** Crane parse ast main execute.
*/
int craP_parsedesc(XokMalloc *xm , lexState *ts , desclist *dl, int curidx){
  /* Package tokens and index(curr) */
 ParseFactor pf = {ts , curidx};
 /* Ast parse executor main while */
 while(pf.curr < ts->count){
  log("loop");
  /* Noted: very loop entry switch match pf's curr have selfadd,
   inner function get GRAM token need back. */
  String *t = pf.tokens->lextokens[pf.curr++]->token;
  log(t->str);
  switch(tokentype(t)){
   case GRAM_UNKEY:{
     craastdesc *cd = unkeydesc(xm , &pf, dl);
     pushdl(xm, dl, cd);
     if(cd == NULL)
      return errorcode;
    break;
   }
   case GRAM_IF:{
     craastdesc *r = ifdesc(xm, &pf, ts);
     if(r->kind == AST_IF){log("R is IF");}
     pushdl(xm, dl, r);
     log("if end");
    break;
   }
   case GRAM_NOTE:{
     int code = notedesc(&pf);
     if(code)return errorcode;
    break;
   }
   case GRAM_EMPTY:{
    break;
   }
   case GRAM_BLOCK_END:{  /* Reach "}" toekn judge is
   "{..}" terminal, use craP_parsedesc() should return */
    return pf.curr;
   }
   case GRAM_VAR:{
     craastdesc *vd = 
     vardesc(xm , &pf , t);
     pushdl(xm , dl , vd);
    break;
   }
  }
 }
 for(int i = 0; i < dl->count; i++){
  if(dl->desclist[i]->kind == AST_IF){log(" this DL is IF");}
  if(dl->desclist[i]->kind == AST_UNKEY){log(" this DL is IF");}
 }
}

