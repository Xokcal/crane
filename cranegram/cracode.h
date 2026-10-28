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
 OP_PUSH,    /* [0]push Value* */
 OP_ADD,     /* [1]calculate add */
 OP_SUB,     /* [2]calculate sub */
 OP_DIV,     /* [3]calculate divide */
 OP_MUL,     /* [4]calculate multiply */
 OP_HALT,    /* [5]vm quit */
 OP_STORE,   /* [6]store variable */
 OP_CALL,    /* [7]execute function call */
 OP_LOAD,    /* [8]args load */
 OP_TYPE     /* [9]store variable type */
}opcode;

typedef enum vartype{
 TYPE_INT,
 TYPE_STRING,
 TYPE_FLOAT,
 TYPE_DOUBLE,
 TYPE_CHAR,
 TYPE_LONG,
 TYPE_UNKOWN
}vartype;

/* store crane var(int , float , long ,string,...)
type value. struct type store var enum vartype */
typedef struct value{
  /* vartype enum type, use to match*/
 vartype type;
 /* variable name like: int a = 10;
                            | 
                            `---------> name*/
 char *vname;
 /* authentic variable type,
 like: {int, double, long, float, string} */
 char *vtype;
 union
 {
   int i;
   float f;
   char *s;
   char c;
   double d;
   long l;
   long long ll;
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

typedef struct varnamestk {
 char **vn; 
 int *slots;
 int size;
 int count;
}varnamestk;

/*
** store variables type when AST parse
*/
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