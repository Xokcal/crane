#ifndef CRANEC_CRAPARSE
#define CRANEC_CRAPARSE

#include <stdio.h>
#include <string.h>
#include "cralex.h"
#include "cracode.h"

typedef struct value value;

typedef enum cragramtype{
  GRAM_INT,       /* [0]origin token is 'int' */
  GRAM_STRING,    /* [1]origin token is 'string' */
  GRAM_FLOAT,     /* [2]origin token is 'float' */
  GRAM_PRINTF,    /* [3]origin token is 'printf' */
  GRAM_EMPTY,     /* [4]origin token is 'empty' */
  GRAM_VAR,       /* [5]origin token is 'variable' */
  GRAM_PRINT,     /* [6]origin token is 'print' */
  GRAM_UNKEY      /* [7]origin token is 'unkey' */
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
  AST_LVNAME        /* [8]AST type left value */
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
 } as;
}craastdesc;

typedef struct desclist{
  craastdesc **desclist;
  int size;
  int count;
}desclist;

typedef struct ParseFactor{
  TOKENSB *tokens;
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
int craP_parsedesc(XokMalloc *xokMalloc , TOKENSB *tokens , desclist *dl);

desclist *craP_desclist_create(XokMalloc *xm, int init_size);

void craP_desclist_free(XokMalloc *xm, desclist *dl);

int craP_desclist_push(XokMalloc *xm, desclist *dl, craastdesc *tree);

int craP_desclist_expand(XokMalloc *xm, desclist *dl);
#endif