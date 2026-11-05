#ifndef CRANEC_CRAPARSE
#define CRANEC_CRAPARSE

#include <stdio.h>
#include <string.h>
#include "cralex.h"
#include "cracode.h"

/* struct create desclist init desc arrays size */
#define DESCLIST_DEFAULT_SIZE 200

/* struct expand number */
#define DESCLIST_GROW_FACTOR  2

#define acptkprint "print"

#define acptkint "int"

/* PH(parenthesee)*/
#define acptlPH "("

#define acptrPH ")"

#define acptempty " "

#define acptSemicolon ";"

/* DQ(double quotaion)*/
#define acptDQ "\""

#define makefnTpysize 2000

#define makefnfdescssize 300
/* ************** String func tackle ***************** */
#define cmptail(s , tail) combine_tail((s), (tail))

#define strclear(s) delete_all((s))

#define strfree(s) string_free((s))
/* ************** struct Parsefactor tackle *************** */
#define pfcurstr(pf) \
 pf->tokens->lextokens[pf->curr]->token->str

#define craP_ps_currtoken(ps) \
(ps).pf->tokens->lextokens[(ps).pf->curr]->token

#define craP_pf_currtoken(pf) \
(pf)->tokens->lextokens[(pf)->curr]->token

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

#define getcurline(pf)\
 (pf)->tokens->lextokens[(pf)->curr]->line

/* current line number */
#define craC_tokencurline(pf)\
 (pf)->tokens->lextokens[(pf)->curr]->line

#define pushdl(xm, dl, t)\
 craP_desclist_push((xm) , (dl) , (t));

typedef struct value value;
typedef struct lexState lexState;

typedef enum cragramtype{
  GRAM_INT,       /* [0]origin token is 'int' */
  GRAM_STRING,    /* [1]origin token is 'string' */
  GRAM_FLOAT,     /* [2]origin token is 'float' */
  GRAM_PRINTF,    /* [3]origin token is 'printf' */
  GRAM_EMPTY,     /* [4]origin token is 'empty' */
  GRAM_VAR,       /* [5]origin token is 'variable' */
  GRAM_UNKEY,     /* [6]origin token is 'unkey' */
  GRAM_NOTE       /* [7]origin token is '-' judge behind is '--' note */
}cragramtype;

/* signed ast descibtion grammar tree type */
typedef enum craasttype{
  AST_VAR,          /* [0]AST type variable */
  AST_NUM,          /* [1]AST type number */
  AST_ASSIGN,       /* [2]AST type assign */
  AST_BINOP,        /* [3]AST type binop(+,-,/,*,..) */
  AST_EMPTY,        /* [4]AST type empty */
  AST_FUNC,         /* [5]AST type function */
  AST_UNKEY,        /* [6]AST type unkey */
  AST_CALL,         /* [7]AST type call(function) */
  AST_LVNAME,       /* [8]AST type left value */
  AST_HASVAR        /* [9]AST type has def variable */
}craasttype;

/* define param type like: {("hello", count, 10086, 3.1415)} */
typedef enum paramtype{
 PRM_STRING,    /* [0]param type 'string' */
 PRM_VAR,       /* [1]param type 'variable' */
 PRM_NUMBER     /* [2]param type 'number' */
}paramtype;

typedef struct Tparam{
 paramtype pty;
 char *originstr;
}Tparam;

typedef struct craastdesc{
 craasttype kind;   /* ast's kind */
 union{
  /*
  ** varname: store is variable token, like: {int i = 1; i++;} the 'a++' 
  ** 'a' is need to stored variable or {int b = 13; int a = b + 1;} 
  ** 'b + 1' can parse to struct desc.var and struct desc.num
  */
  char *varname;   /* ast's variable name */
  char *lvname;    /* left value name like: {int a = v * 3;}  'v' is lvname */
  /*
  ** num: store variable token is number comdition
  */
  int num;
  struct binop{
   struct craastdesc *right;   /* binop's right */
   struct craastdesc *left;    /* binop's left */
   char *calcsign;             /* binop's calculate aign */
  } binop;
  struct assign{
   struct craastdesc *value;   /* assign value */
   /* origin text data type(str), like: int a; --> store string "int"*/
   char *type;
   char *name;     /* assign about of variable name */
  } assign;
  struct call{          /* function call */
   struct Tparam *args; /* args arrays */
   char *fname;         /* function name */
   int argsi;           /* args count */
   /* modified array define .. */
  }call;
  struct hasvar{             /* has def variable reassign */
   char *name;               /* variable name */
   struct craastdesc *value; /* value tree*/
   char *type;               /* variable type */
  }hasvar;
 } as;
}craastdesc;

#define HIGH_PRI_SIGN_SIZE 2
#define LOW_PRI_SIGN_SIZE 2

static char *highprisign[] = {"*" , "/"};

static char *lowprisign[] = {"+" , "-"};

typedef struct desclist{
  craastdesc **desclist;
  int size;
  int count;
}desclist;

typedef struct ParseFactor{
  lexState *tokens;
  int curr;
}ParseFactor;

typedef struct AstDescArr{
  craastdesc **descs;
  int count;
  int size;
}AstDescArr;

/* parse state: xm(memery pool),
pf(tokens), root(desc tree root) */
typedef struct pavstate{
  XokMalloc *xm;
  ParseFactor *pf;
  craastdesc *root;
}pavstate;

/* crane ast parse main funtion, xokMalloc is memory pool */
int craP_parsedesc(XokMalloc *xokMalloc , lexState *tokens , desclist *dl);
desclist *craP_desclist_create(XokMalloc *xm, int init_size);
void craP_desclist_free(XokMalloc *xm, desclist *dl);
int craP_desclist_push(XokMalloc *xm, desclist *dl, craastdesc *tree);
int craP_desclist_expand(XokMalloc *xm, desclist *dl);
#endif