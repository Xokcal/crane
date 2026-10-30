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

#define acptkprint "print"

#define acptkint "int"

/* PH(parenthesee)*/
#define acptlPH "("

#define acptrPH ")"

#define acptempty " "

#define acptSemicolon ";"

/* DQ(double quotaion)*/
#define acptDQ "\""

#define makefnTpysize 200

#define makefnfdescssize 300
/* ************** String func tackle ***************** */
#define cmptail(s , tail) combine_tail((s), (tail))

#define strclear(s) delete_all((s))

#define strfree(s) string_free((s))
/* ************** struct Parsefactor tackle *************** */
#define pfcurstr(pf) \
 (pf)->tokens->tokens[(pf)->curr]->str

#define craP_ps_currtoken(ps) \
(ps).pf->tokens->tokens[(ps).pf->curr]

#define craP_pf_currtoken(pf) \
(pf)->tokens->tokens[(pf)->curr]

#define pfnext(pf) (pf)->curr++

#define pfback(pf) (pf)->curr--

/* ************* desccall tackle *************** */
#define desccallpushname(desc , name) \
 (desc)->as.call.fname = (unkey);

/* Tparam push desc.func args stack. */
#define callpushargs(desc , tp)\
 (desc)->as.call.args[desc->as.call.argsi++] = (tp);

/* ************** common ******************* */
#define craC_isstrvar(s) \
 !isnumber((s)) && !strcmp((s), acptDQ)==0 \
 && !strcmp((s), acptempty)==0 && !cmpkey((s))

#define pushdl(xm, dl, t)\
 craP_desclist_push((xm) , (dl) , (t));



/*
** judge current token char* whither is high priority 
** sign or low priority sign.
*/
static int
ishighprioritysign(char *n)
{
 for(int i=0;i<HIGH_PRI_SIGN_SIZE;i++){
  if(strcmp(n, highprisign[i])==0)return 1;
 }return 0;
}


static int
islowprioritysign(char *n)
{
 for(int i=0;i<LOW_PRI_SIGN_SIZE;i++){
  if(strcmp(n, lowprisign[i])==0)return 1;
 }return 0;
}


/*
** token convert enum asttype, return to tree parse switch. 
*/
static int
tokentype(String *s)
{
  if(compare(s, "int")||compare(s, "float")
  ||compare(s, "double")||compare(s, "char")
  ||compare(s, "long"))return GRAM_VAR;
  if(compare(s , " "))return GRAM_EMPTY;
  if(compare(s , "\n"))return GRAM_EMPTY;
  if(compare(s , "\r"))return GRAM_EMPTY;
  if(compare(s , ";"))return GRAM_EMPTY;
  if(compare(s , "print"))return GRAM_PRINT;
  return GRAM_EMPTY;
}


/*
** judge string whither is number: isnumber return 1, nonumber return 0.
*/
static int
isnumber(const char *str) {
 if (str == NULL || *str == '\0') return 0;
 while (*str != '\0') {
  if (!isdigit(*str)) return 0;
  str++;
 }
 return 1;
}


static inline int
accept(ParseFactor *pf , char *name)
{
 if(compare(pf->tokens->tokens[pf->curr] , name)){
   pf->curr++;
   return 1;
 }
 return 0;
}


static inline void
accept_unkey(ParseFactor *pf){pf->curr++;}


static void
accept_empty(ParseFactor *pf)
{
 while(pf->curr < pf->tokens->count)
 {
   String *s = pf->tokens->tokens[pf->curr];
   if(compare(s , " "))pf->curr++;
   else return;
 }
}


/* test whither have next like: {args, assign, ..}*/
static inline int
testnext(ParseFactor *pf , char *s)
{
 accept_empty(pf); /* check current pf not is empty key */
 int bl = strcmp(pfcurstr(pf), s) == 0 ? 1 : 0;
 pfnext(pf);
 return bl;
}


/*
** parse 
*/
static inline pavstate
pavstate_init(XokMalloc *xm,ParseFactor *pf)
{
 pavstate pavs;
 pavs.xm = xm;
 pavs.pf = pf;
 pavs.root = NULL;
 return pavs;
}


static void
print_ast(craastdesc *node, int depth)
{
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
** ast parse variable string name to struct desc
*/
static craastdesc*
make_var(XokMalloc *xokmalloc , char *name)
{
 craastdesc *node = (craastdesc *)
 xalloc_XokMalloc(xokmalloc , sizeof(craastdesc));
 node->kind = AST_VAR;
 node->as.varname = strdup(name);  /* remember free node name */
 return node;
}

static craastdesc* 
make_num(XokMalloc *xokMalloc ,  int n)
{
 craastdesc *node = (craastdesc *)
 xalloc_XokMalloc(xokMalloc , sizeof(craastdesc));
 node->kind = AST_NUM;
 node->as.num = n;
 return node;
}

static craastdesc *
make_binop(XokMalloc *xokMalloc, char *calcsign 
    , craastdesc *left , craastdesc *right)
{
 craastdesc *node = (craastdesc *)
 xalloc_XokMalloc(xokMalloc , sizeof(craastdesc));
 node->kind = AST_BINOP;
 node->as.binop.calcsign = strdup(calcsign); /* remember free node.left! */
 node->as.binop.left = left; 
 node->as.binop.right = right;
 return node;
}

static craastdesc*
make_assign(XokMalloc *xokMalloc 
, char *name , craastdesc *value , char *type)
{
 craastdesc *node = (craastdesc *)
 xalloc_XokMalloc(xokMalloc , sizeof(craastdesc));
 node->kind = AST_ASSIGN;
 node->as.assign.name = strdup(name);
 node->as.assign.value = value;
 node->as.assign.type = strdup(type);
 return node;
}

static craastdesc* 
make_call(XokMalloc *xokMalloc, char *name)
{
 craastdesc *node = (craastdesc *)
 xalloc_XokMalloc(xokMalloc , sizeof(craastdesc));
 node->as.call.args = (Tparam *)
 xalloc_XokMalloc(xokMalloc , sizeof(Tparam) * makefnTpysize);
 node->kind = AST_CALL;
 node->as.call.fname = strdup(name);
 node->as.call.argsi = 0;
 return node;
}

static craastdesc*
make_lvalue(XokMalloc *xokmalloc , char *name)
{
 craastdesc *node = (craastdesc *)
 xalloc_XokMalloc(xokmalloc , sizeof(craastdesc));
 node->kind = AST_LVNAME;
 node->as.lvname = strdup(name);  /* remember free node name */
 return node;
}


/*
****************** desclist cycle funtion**********************
*/

/* struct create desclist init desc arrays size */
#define DESCLIST_DEFAULT_SIZE 200

/* struct expand number */
#define DESCLIST_GROW_FACTOR  2

/*
** Create a description list with initial capacity 'init_size'.
** A non-positive 'init_size' uses DESCLIST_DEFAULT_SIZE.
** Returns NULL if allocation fails.
*/
desclist *
craP_desclist_create(XokMalloc *xm, int init_size)
{
  if (init_size <= 0) init_size = DESCLIST_DEFAULT_SIZE;
  desclist *dl = (desclist *)xalloc_XokMalloc(xm, sizeof(desclist));
  if (!dl) return NULL;
  dl->desclist = (craastdesc **)xalloc_XokMalloc(
  xm, sizeof(craastdesc *) * init_size);
  if (!dl->desclist)return NULL; /* dl non-create return NULL */
  dl->size  = init_size;
  dl->count = 0;
  return dl;
}


inline void
craP_desclist_free(XokMalloc *xm, desclist *dl)
{
  if (!dl) return;
  if (dl->desclist)dl->desclist = NULL;
}


/*
** Desc list expand size by (origin size * DESCLIST_GROW_FACTOR).
** memcpy function copy dl's origin desclist and give to new desclist.
** need't free origin desclist, it use xokmalloc pool. 
*/
int
craP_desclist_expand(XokMalloc *xm, desclist *dl)
{
  if (!dl) return 0;
  int new_size = dl->size * DESCLIST_GROW_FACTOR;
  craastdesc **new_arr = (craastdesc **)xalloc_XokMalloc(
  xm, sizeof(craastdesc *) * new_size);
  if (!new_arr) return 0; /* non-create return 0. */
  memcpy(new_arr, dl->desclist, sizeof(craastdesc *) * dl->count);
  dl->desclist = new_arr;
  dl->size = new_size;
  return 1; /* success return 1. */
}



/*
** new desc tree push desclist stack, if dl count >= dl size
** use desclist expand funtion.
*/
int
craP_desclist_push(XokMalloc *xm, desclist *dl, craastdesc *tree)
{
  if (!dl) return 0;
  if (dl->count >= dl->size) {
  if (!craP_desclist_expand(xm, dl))return 0;}
  dl->desclist[dl->count++] = tree;
  return 1;
}


/*
** variable is undefine status, parse to not have assign value's expdesc
*/
static craastdesc*
pushvarundefine(XokMalloc *xm , String *name , String *type)
{return make_assign(xm , name->str , NULL , type->str);}


/*
** assign value expdesc node parse factor, assign value's number, function, 
** arrays index value,... parse to desc tree and return mid high priority
** and low priority function tackle.
*/
static craastdesc*
parse_factor(pavstate ps)
{
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



static craastdesc*
parse_mid(pavstate ps)
{
 accept_empty(ps.pf);
 /* low priority calculate sign ,tackle struct 'craastdesc*' */
 craastdesc *l = parse_factor(ps); /* is 'main node' in the funtion */
 String *cur = craP_ps_currtoken(ps);
 /* continuous tackle high priority condition */
 while(ishighprioritysign(cur->str)){
  pfnext(ps.pf);
  accept_empty(ps.pf); /* current pf.token is calcsign */
  /* ps entry parse_factor ps root address convert, but previous
  ps root's desc result address is store to craastdesc *l */
  craastdesc *r = parse_factor(ps);
  l = make_binop(ps.xm , cur->str , l , r);
  /* every loop, l is after refresh l and loop internal r(num) 
  make_binop() overall root node */
  cur = craP_ps_currtoken(ps); /* refresh current calcsign */
 }
 /* external funtion need r node, l is has tackled all continuous high priority
 node tree, and/or first entre the funtion's low priority node(tree) */
 return l;
}



static craastdesc*
parse_value_main(pavstate ps)
{
 accept_empty(ps.pf);
 /* tackle low priority or continous tackle high priority,
  use ps.root to accept res , root is current all craastdesc node */
 ps.root = parse_mid(ps); 
 /* drawer parse_mid() , current caculate sign */
 String *cur = craP_ps_currtoken(ps);
 while (islowprioritysign(cur->str)){
  pfnext(ps.pf);
  accept_empty(ps.pf); /* current is match right number */
  craastdesc *r = parse_mid(ps);
  ps.root = make_binop(ps.xm , cur->str , ps.root , r);
  /* ps.root is previous ps.root and craastdesc *r binop node 
  (about parse_mid() internal high priority root accept overall node),
  and current pf.token is calcsign */
  cur = craP_ps_currtoken(ps); /* refresh current calcsign */
 }
 /* root is main node, has collected all tackled node tree */
 return ps.root; 
}



/*
** variable parse to expdesc main function.
** about differ variable type, and differ variable garmmar express type.
*/
static craastdesc*
var_desc(XokMalloc *xm , ParseFactor *pf, String *t)
{
 String *type = create_string(t->str);
 accept(pf , acptkint);  /* int type var */
 accept_empty(pf);  /* throught empty to variable name */
 String *name = craP_pf_currtoken(pf);
 pfnext(pf);
 craastdesc* var = make_var(xm , name->str);  /* variable's name */
 accept_empty(pf);  /* throught empty to '=' */
 int isEqu = accept(pf , "=");  /* judge '=' */
 if(!isEqu){  /* if isEqu not's true , declare is 'int a;' */
  craastdesc *undefinevar = 
  pushvarundefine(xm , name , type);  /* undefine variable tackle */
  return undefinevar;
 }
 pavstate ps = pavstate_init(xm , pf);
 craastdesc *v = parse_value_main(ps); /* tackle after '=' value tree */
 /* current 'desc' struction is: struct assign* = {var_name , struct binop* root} */
 craastdesc *desc 
 = make_assign(xm , name->str , v , type->str);
 strfree(t);
 return desc;
}



/*
** function str param type, record like: {"hello crane!"}
** originstr = "hello crane!", and tackle print in virtual machine
** package by struct Tparam.
*/
static void
stringargs(ParseFactor *pf, craastdesc *desc, String *t)
{
 if(strcmp(pfcurstr(pf) , "\"")==0){ /* is string */
  accept(pf , "\""); /* eat '|"' */
  while(strcmp(pfcurstr(pf) , "\"") != 0){
   cmptail(t, pfcurstr(pf)); /* main content */
   pfnext(pf);
  }/* current pf.token is double parenthesee , need slip the 
  token by pfnext() later */
  Tparam tp;
  tp.pty = PRM_STRING;
  tp.originstr = strdup(t->str);
  /* Tparam tp push desc func args, and selfadd argsi. */
  callpushargs(desc, tp);
  //#define strorigin
  #ifdef strorigin
   logc("string origin string" , tp.originstr);
  #endif
  strclear(t);  /* template String* delete all content. */
  pfnext(pf);  /* slip '\"'. */
  accept_empty(pf);  /* current chould is ',' or ')'  */ 
 }
}



static void
singletokenargs(ParseFactor *pf, craastdesc *desc)
{
/* could is ',' or crane key
 if the funtion have more args, and string args not is last args
 like print(a , "crane" , 13)
                        `--------> ',' (be thought it was variable args). */
 if(cmpkey(pfcurstr(pf)))return; 
 Tparam tp;
 /* single string param about 'number', 'variable', 'enum', ..
 judge store param type by condition judgement. */
 if(craC_isstrvar(pfcurstr(pf)))
 tp.pty = PRM_VAR;
 else if(isnumber(pfcurstr(pf)))
 tp.pty = PRM_NUMBER;
/*  store origin string, convert number type(int, long, float, double, 
 ..) in virtual machine. */
 tp.originstr = strdup(pfcurstr(pf));
 callpushargs(desc, tp);
 pfnext(pf);
 accept_empty(pf);
}



static int
unkey_desc_fn(ParseFactor *pf, craastdesc *desc
                  , String *t, char *unkeytype){
 if(!strcmp(unkeytype , "(")){ /* "(" is function! */
  pfnext(pf);
  do{
   if(!strcmp(pfcurstr(pf) , ")"))
   /* if function not have args like: "print();"
   , next token is ')' can return. */
   return successcode;
   /* current is '(' next token */
   accept_empty(pf);  /* confirm not is empty, reach first param */
   stringargs(pf , desc , t);
   singletokenargs(pf , desc);
  }while(testnext(pf, ","));  /* if have one or more args, at least 
  loop one time, have ',' execute next args and selfadd pf.curr. */
  if(accept(pf , acptSemicolon))
   return successcode;
  craCM_error("function not have end: \";\" ");
  return errorcode;
 }
}



/* 
** unkey about like function, selfadd, selfsub,..,the unkey need 
** special tackle, unkey not is key, and not is queto, parenthesee,
** .., so overall call these is'unkey'.
*/
static craastdesc*
unkey_desc(XokMalloc *xm, ParseFactor *pf)
{
 pfback(pf);  /* current pf is slip key,
 pf need back to previous index get varibale key */
 int unkeycode; /* accept unkey_desc return code*/
 craastdesc *desc; /* overall desc node */
 /* str tool, convenient to tackle str */
 String *t = create_string("");        
 char *unkey = strdup(pfcurstr(pf)); /* store function name */
 pfnext(pf); /* next is {'(', '++', '--', "+=", '-=', ...}, 
 judge unkey specific type  */
 if(!strcmp(pfcurstr(pf) , "("))
  desc = make_call(xm, unkey);
  /* store, use to judge unkey how about grammar type*/
 char *unkeytype = strdup(pfcurstr(pf)); 
 unkeycode = unkey_desc_fn(pf, desc, t, unkeytype); /* priority judge function */
 if(unkeycode == errorcode)return NULL;
 //#define descfuncprt
 #ifdef descfuncprt    /* check desc.func's args variable name */
 logi("argsi" , desc->as.call.argsi);
 for(int i=0;i<desc->as.call.argsi;i++){
  logc("function args origin",desc->as.call.args[i].originstr);
  logi("function args type",desc->as.call.args[i].pty);
 }
 #endif
 free(unkeytype);
 strfree(t);
 return desc;
}



/*
** crane parse ast main execute.
*/
int
craP_parsedesc(XokMalloc *xm , TOKENSB *ts , desclist *dl)
{
 ParseFactor pf = {ts , 0};  /* package tokens and index(curr) */
 while(pf.curr < ts->count){  /* ast parse executor main while */
  String *t = pf.tokens->tokens[pf.curr++];
  switch(tokentype(t)){
   case GRAM_PRINT:{
    craastdesc *cd = unkey_desc(xm , &pf);
    pushdl(xm, dl, cd);
    if(cd == NULL)
     return errorcode;
    break;
   }
   case GRAM_EMPTY:{
    break;
   }
   case GRAM_VAR:{
    craastdesc *vd = var_desc(xm , &pf , t);
    pushdl(xm , dl , vd);
    break;
   }
  }
 }
}

