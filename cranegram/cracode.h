#ifndef cracode_h
#define cracode_h

#include <stdio.h>
#include "../tool/xokmalloc/xokmalloc.h"
#include "craparse.h"
#include "cravm.h"

typedef struct desclist desclist;
typedef struct Tparam Tparam;

/* virtual machine instruct */
typedef enum opcode{
 OP_PUSH,      /* [0]push Value* */
 OP_ADD,       /* [1]calculate add */
 OP_SUB,       /* [2]calculate sub */
 OP_DIV,       /* [3]calculate divide */
 OP_MUL,       /* [4]calculate multiply */
 OP_HALT,      /* [5]vm quit */
 OP_STORE,     /* [6]store variable */
 OP_CALL,      /* [7]execute function call */
 OP_LOAD,      /* [8]args load */
 OP_TYPE,      /* [9]store variable type */
 OP_PUSH_VAR   /* [10]push variable */
}opcode;

/*
** use to various need to judged variable type condition
*/
typedef enum vartype{
 TYPE_INT,      /* [0]type int */
 TYPE_STRING,   /* [1]type string */
 TYPE_FLOAT,    /* [2]type float */
 TYPE_DOUBLE,   /* [3]type double */
 TYPE_CHAR,     /* [4]type char */
 TYPE_LONG,     /* [5]type long */
 TYPE_DLONG,    /* [6]type longlong */
 TYPE_UNKOWN    /* [7]not kown type */
}vartype;

/* store crane variable(int , float , long ,string,...)
type value. struct type store variable enum vartype */
typedef struct value{
  /* vartype enum type, use to match*/
 vartype type;
 /* variable name like: int a = 10;
                            | 
                            `---------> name*/
 char *vname; /* variable name string value */
 char *vtype; /* authentic variable type,
 like: {int, double, long, float, string} */
 union
 {
   int        i;     /* authentic variable type 'int' */
   float      f;     /* authentic variable type 'float' */
   char       *s;    /* authentic variable type 'string' */
   char       c;     /* authentic variable type 'char' */
   double     d;     /* authentic variable type 'double' */
   long       l;     /* authentic variable type 'long' */
   long long  ll;    /* authentic variable type 'longlong' */
 }v;
}value;

typedef struct bytecode{
 int *codes;
 int count;
 int size;
 value *values;
 int v_count;
 int v_size;
}bytecode;

/* store variable string name array */
typedef struct varnamestk {
 char **vn; 
 int *slots;
 int size;
 int count;
}varnamestk;

/* store variables type when AST parse*/
typedef struct Ttypes{
 char **types;
 int size;
 int count;
}Ttypes;

/* call args store stack */
typedef struct Targs{
 Tparam *args;
 int size;
 int count;
}Targs;

/* call name store stack */
typedef struct Tcalls{
 char **calls;
 int size;
 int count;
}Tcalls;

vmState*
craC_bytecode_parse(XokMalloc *xm ,  desclist *dl);

bytecode *bc_create(XokMalloc *xm, int init_size);

void bc_free(XokMalloc *xm, bytecode *bc);

int bc_expand(XokMalloc *xm, bytecode *bc);

int bc_emit(XokMalloc *xm, bytecode *bc, int instr);

int bc_add_constant(XokMalloc *xm, bytecode *bc, value v);

const char* typetostr(vartype type);

#endif