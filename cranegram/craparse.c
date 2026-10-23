#include <stdio.h>
#include <ctype.h>
#include "craparse.h"
#include "../tool/xokmalloc/xokmalloc.h"
#include "cgm_parse.h"
#include "cgm_gramma.h"
#include "../tool/String/String.h"
#include "../tool/log/log.h"
#include "cgm_file.h"
#include "cracode.h"
#include "cravm.h"

#define acptkprint "print"

#define acptkint "int"

/* PH(parenthesee)*/
#define acptlPH "("

#define acptrPH ")"

#define acptempty " "

/* DQ(double quotaion)*/
#define acptDQ "\""

#define makefnTpysize 16

#define makefnfdescssize 50
/* ************** String func tackle ***************** */
#define cmptail(s , tail) combine_tail((s), (tail))

#define strclear(s) delete_all((s));

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

/* ************* descfn tackle *************** */
#define descfnpushname(desc , name) \
 (desc)->as.func.fname = (unkey);

#define descfnpushpusharg(desc , tp)\
 (desc)->as.func.args[desc->as.func.argsi++] = (tp);

/* ************** common ******************* */
#define craC_isstrvar(s) \
 !isnumber((s)) && !strcmp((s), acptDQ)==0 \
 && !strcmp((s), acptempty)==0 && !cmpkey((s))


static int
t_convert_asttype(String *s)
{
  if(compare(s, "int")     || 
    compare(s, "float")     || 
    compare(s, "double")    || 
    compare(s, "char")      || 
    compare(s, "long"))
    return GRAM_VAR;
  if(compare(s , " "))return GRAM_EMPTY;
  if(compare(s , "\n"))return GRAM_EMPTY;
  if(compare(s , ";"))return GRAM_EMPTY;
  if(compare(s , "print"))return GRAM_PRINT;
}


static int
isnumber(const char *str) {
 if (str == NULL || *str == '\0') return 0;
 while (*str != '\0') {
  if (!isdigit(*str)) return 0;
  str++;
 }
 return 1;
}


static int
accept(ParseFactor *pf , char *name)
{
 if(compare(pf->tokens->tokens[pf->curr] , name)){
   pf->curr++;
   return 1;
 }
 return 0;
}


static void
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
static int
testnext(ParseFactor *pf , char *s)
{
 accept_empty(pf); /* check current pf not is empty key */
 int bl = strcmp(pfcurstr(pf), s) == 0 ? 1 : 0;
 pfnext(pf);
 return bl;
}


static void
pav_clear_string(String *num_p1 , String *num_p2 
, String *calcsign , String *calcsign2)
{
 delete_all(num_p1);
 delete_all(num_p2);
 delete_all(calcsign);
 delete_all(calcsign2);
}


static pavstate
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

static
craastdesc *make_var(XokMalloc *xokmalloc , char *name)
{
 craastdesc *node = (craastdesc *)
 xalloc_XokMalloc(xokmalloc , sizeof(craastdesc));
 node->kind = AST_VAR;
 node->as.varname = strdup(name);  /* remember free node name */
 return node;
}

static
craastdesc *make_num(XokMalloc *xokMalloc ,  int n)
{
 craastdesc *node = (craastdesc *)
 xalloc_XokMalloc(xokMalloc , sizeof(craastdesc));
 node->kind = AST_NUM;
 node->as.num = n;
 return node;
}

static
craastdesc *make_binop(XokMalloc *xokMalloc, char *calcsign 
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

static
craastdesc *make_assign(XokMalloc *xokMalloc 
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

static
craastdesc *make_func(XokMalloc *xokMalloc, char *name)
{
 craastdesc *node = (craastdesc *)
 xalloc_XokMalloc(xokMalloc , sizeof(craastdesc));
 node->as.func.args = (Tparam *)
 xalloc_XokMalloc(xokMalloc , sizeof(Tparam) * makefnTpysize);
 node->as.func.fdescs = (craastdesc *)
 xalloc_XokMalloc(xokMalloc , sizeof(craastdesc) * makefnfdescssize);
 node->kind = AST_FUNC;
 node->as.func.fname = strdup(name);
 node->as.func.argsi = 0;
 node->as.func.fdesci = 0;
 return node;
}


/*
****************** desclist cycle funtion**********************
*/
#define DESCLIST_DEFAULT_SIZE 16
#define DESCLIST_GROW_FACTOR  2

desclist *
Cra_desclist_create(XokMalloc *xm, int init_size)
{
  if (init_size <= 0) init_size = DESCLIST_DEFAULT_SIZE;
  desclist *dl = (desclist *)xalloc_XokMalloc(xm, sizeof(desclist));
  if (!dl) return NULL;
  dl->desclist = (craastdesc **)xalloc_XokMalloc(
      xm, sizeof(craastdesc *) * init_size);
  if (!dl->desclist)return NULL;
  dl->size  = init_size;
  dl->count = 0;
  return dl;
}

/* 释放：释放数组 + 释放结构体 */
void
Cra_desclist_free(XokMalloc *xm, desclist *dl)
{
  if (!dl) return;if (dl->desclist)dl->desclist = NULL;
}


int
Cra_desclist_expand(XokMalloc *xm, desclist *dl)
{
  if (!dl) return 0;
  int new_size = dl->size * DESCLIST_GROW_FACTOR;
  craastdesc **new_arr = (craastdesc **)xalloc_XokMalloc(
      xm, sizeof(craastdesc *) * new_size);
  if (!new_arr) return 0;
  memcpy(new_arr, dl->desclist, sizeof(craastdesc *) * dl->count);
  dl->desclist = new_arr;
  dl->size     = new_size;
  return 1;
}

int
Cra_desclist_push(XokMalloc *xm, desclist *dl, craastdesc *tree)
{
  if (!dl) return 0;
  if (dl->count >= dl->size) {
    if (!Cra_desclist_expand(xm, dl))return 0;
  }
  dl->desclist[dl->count++] = tree;
  return 1;
}


/*
******************************************************************
*                      AST gramma parse                          *
******************************************************************
*/
static craastdesc*
push_var_undefine(XokMalloc *xm , String *name , String *type)
{return make_assign(xm , name->str , NULL , type->str);}


static craastdesc*
parse_factor(pavstate ps)
{
  /* tackle curr number to struct 'craastdesc*' */
  int *n = str_convert_int(craP_ps_currtoken(ps));
  ps.pf->curr++;
  ps.root = make_num(ps.xm , *n);
  /* pf's index step to next unempty token */
  accept_empty(ps.pf);
  free(n);  /* temp free n , will use memory pool later */
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
  while(compare(cur , "*") || compare(cur , "/")){
    ps.pf->curr++;
    accept_empty(ps.pf);  /* current pf.token is calcsign */
    craastdesc *r = parse_factor(ps);
    l = make_binop(ps.xm , cur->str , l , r); /* every loop, l is 
    after refresh l and loop internal r(num) make_binop() overall root node  */
    cur = craP_ps_currtoken(ps); /* refresh current calcsign */
  }
  return l; /* external funtion need r node,
  l is has tackled all continuous high priority node tree,
  and/or first entre the funtion's low priority node(tree) */
}


static craastdesc*
parse_value_main(pavstate ps)
{
  accept_empty(ps.pf);
  ps.root = parse_mid(ps); /* tackle low priority or 
  continous tackle high priority,
  use ps.root to accept res , root is current all craastdesc node*/
  String *cur = craP_ps_currtoken(ps); /* drawer parse_mid() , current caculate sign */
  while (compare(cur , "+") || compare(cur , "-")){
    ps.pf->curr++;
    accept_empty(ps.pf); /* current is match right number */
    craastdesc *r = parse_mid(ps);
    ps.root = make_binop(ps.xm , cur->str , ps.root , r); 
    /* ps.root is previous ps.root and craastdesc *r binop node 
    (about parse_mid() internal high priority root accept overall node),
    and current pf.token is calcsign */
    cur = craP_ps_currtoken(ps); /* refresh current calcsign */
  }
  return ps.root; /* root is main node, 
  has collected all tackled node tree */
}


/* 
** Crane 'int' grammar fule , parse to craastdesc tree.
** return single tree ,collect to tree arrays.
** about [int a = 10 + 2;](define) && [int a;](undefine) &&
** [type a[10] = {typev1 , typev2 , typev3,...typev10}](array var type)...
** contain type have {int , long , long long , char , char*(string),
** float , double , bite}.
 */
static craastdesc*
var_desc(XokMalloc *xm , ParseFactor *pf , String *type)
{
 accept(pf , acptkint);   /* int type var */
 accept_empty(pf);   /* throught empty to variable name */
 String *name = craP_pf_currtoken(pf);
 pf->curr++;
 craastdesc* var = make_var(xm , name->str); /* variable's name */
 accept_empty(pf); /* throught empty to '=' */
 int isEqu = accept(pf , "="); /* judge '=' */
 if(!isEqu){ /* if isEqu not's true , declare is 'int a;' */
  craastdesc *undefinevar = 
  push_var_undefine(xm , name , type);  /* undefine variable tackle */
  return undefinevar;
 }
 pavstate ps = pavstate_init(xm , pf);
 craastdesc *v = parse_value_main(ps); /* tackle after '=' value tree */
 //print_ast(v , 0);
 craastdesc *desc = 
 make_assign(xm , name->str , v , type->str); /* current 'desc' struction is:
 struct assign* = {var_name , struct binop* root} */
 return desc;
}


static void
desc_fn_str(ParseFactor *pf, craastdesc *desc, String *t)
{
 if(strcmp(pfcurstr(pf) , acptDQ)==0){ /* is string */
  accept(pf , acptDQ); /* eat '|"' */
  while(strcmp(pfcurstr(pf) , acptDQ) != 0){
   cmptail(t, pfcurstr(pf)); /* main content */
   pfnext(pf);
  }
  Tparam tp;
  tp.pty = PRM_STRING;
  tp.originstr = strdup(t->str);
  descfnpushpusharg(desc, tp);
  strclear(t);
  accept(pf , acptDQ);
  accept_empty(pf);
 }
}


static void
desc_fn_var(ParseFactor *pf, craastdesc *desc)
{
 if(craC_isstrvar(pfcurstr(pf))){ /* is var */
  Tparam tp;
  tp.pty = PRM_VAR;
  tp.originstr = strdup(pfcurstr(pf));
  descfnpushpusharg(desc, tp);
  pfnext(pf);
  accept_empty(pf);
 }
}


static void
desc_fn_number(ParseFactor *pf, craastdesc *desc)
{
 if(isnumber(pfcurstr(pf))){
  Tparam tp;
  tp.pty = PRM_NUMBER;
  tp.originstr = strdup(pfcurstr(pf));
  descfnpushpusharg(desc, tp);
  pfnext(pf);
  accept_empty(pf);
 }
}


static craastdesc*
unkey_desc_fn(ParseFactor *pf, craastdesc *desc
                  , String *t, char *unkeytype){
 if(!strcmp(unkeytype , acptlPH)){ /* "(" is function! */
  pfnext(pf);
  do{
   accept_empty(pf);
   desc_fn_str(pf , desc , t);
   desc_fn_var(pf , desc);
   desc_fn_number(pf , desc);
  }while(testnext(pf, ","));
 }
}


static craastdesc*
unkey_desc(XokMalloc *xm, ParseFactor *pf)
{
 /* parse_main function loop match token, 
 slip current token when match*/
 pfback(pf);
 craastdesc *desc;
 String *t = create_string("");
 char *unkey = strdup(pfcurstr(pf));
 desc = make_func(xm, unkey);
 accept_unkey(pf);
 accept_empty(pf);
 char *unkeytype = strdup(pfcurstr(pf));
 unkey_desc_fn(pf, desc, t, unkeytype);
 free(unkeytype);
 strfree(t);
 return desc;
}


void
craP_token_parse(XokMalloc *xm , TOKENSB *ts , desclist *dl)
{
 ParseFactor pf = {ts , 0};
 while(pf.curr < ts->count){
  String *t = pf.tokens->tokens[pf.curr++];
  printf("(t):%s\n" , t->str);
  switch(t_convert_asttype(t)){
   case GRAM_PRINT:{
    craastdesc *fd = unkey_desc(xm , &pf);
   }
   case GRAM_EMPTY:{
    pf.curr++;
    break;
   }
   case GRAM_VAR:{
    String *type = create_string(t->str);
    craastdesc *vd = var_desc(xm , &pf , type);
    string_free(type);
    print_ast(vd , 0);
    Cra_desclist_push(xm , dl , vd);
    break;
   }
  }
 }
}

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
  print_welcome();
  String *origin_str = open_file("Crane/test.cr");
  TOKENSB *tokens = tokens_parse(origin_str);
  for(int i = 0; i < tokens->count ; i++){
      //logc("token" , tokens->tokens[i]->str);
  }printf("\n");
  XokMalloc *xm = create_XokMalloc();
  Hashmap *hmap = create_hashmap(); /* gloab variable*/
  desclist *dl = Cra_desclist_create(xm , 16); /* tree list */
  craP_token_parse(xm , tokens , dl);  /*  AST main parse */
  vm *vm = craC_bytecode_parse(xm , dl);
  craV_execute(xm , vm);
  /* free setting struct */
  Cra_desclist_free(xm , dl);
  free_XokMalloc(xm);
  return 0;
}