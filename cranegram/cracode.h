#ifndef cracode_h
#define cracode_h

#include <stdio.h>
#include "../tool/xokmalloc/xokmalloc.h"
#include "craparse.h"
#include "cravm.h"

typedef struct desclist desclist;

typedef enum opcode{
 OP_PUSH,
 OP_ADD,
 OP_SUB,
 OP_DIV,
 OP_MUL,
 OP_HALT,
 OP_STORE
}opcode;

typedef enum vartype{
 TYPE_INT,
 TYPE_STRING,
 TYPE_FLOAT,
 TYPE_DOUBLE,
 TYPE_CHAR,
 TYPE_LONG
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


vm*
craC_bytecode_parse(XokMalloc *xm ,  desclist *dl);

bytecode *bc_create(XokMalloc *xm, int init_size);

void bc_free(XokMalloc *xm, bytecode *bc);

int bc_expand(XokMalloc *xm, bytecode *bc);

int bc_emit(XokMalloc *xm, bytecode *bc, int instr);

int bc_add_constant(XokMalloc *xm, bytecode *bc, value v);

const char* typetostr(vartype type);

#endif