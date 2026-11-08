#ifndef cracode_h
#define cracode_h

#include <stdio.h>
#include "../tool/xokmalloc/xokmalloc.h"
#include "craparse.h"
#include "cravm.h"
#include "craobject.h"

typedef struct desclist desclist;
typedef struct craastdesc craastdesc;
typedef struct Tparam Tparam;

/* virtual machine instruct */
typedef enum opcode{
 OP_PUSH,          /* [0]push Value* */
 OP_ADD,           /* [1]calculate add */
 OP_SUB,           /* [2]calculate sub */
 OP_DIV,           /* [3]calculate divide */
 OP_MUL,           /* [4]calculate multiply */
 OP_HALT,          /* [5]vm quit */
 OP_STORE,         /* [6]store variable */
 OP_CALL,          /* [7]execute function call */
 OP_LOAD,          /* [8]args load */
 OP_TYPE,          /* [9]store variable type */
 OP_PUSH_VAR,      /* [10]push variable */
 OP_STORE_HASVAR,  /* [11]reassign has store variable*/
 OP_JMP_FALSE,     /* [12]behind 'and', if stack is '1' continue */
 OP_JMP_TRUE,      /* [13]behind 'or', if stack is '1' entry */
 OP_JMP            /* [14]jump to specified vm index */
}opcode; 

/*
** package codes execution args
*/
typedef struct codesState{
 XokMalloc *xm;
 Ttypes *types; 
 bytecode *bc;
 varnamestk *vns;
 Targs *targs;
 Tcalls *tcalls;
 int *dl_c;
 int *t_idx;
 int *vnsi;
 int *tcallsi;
}codesState;

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

vmState*craC_bytecode_parse(XokMalloc *xm ,  desclist *dl);
bytecode *bc_create(XokMalloc *xm, int init_size);
void bc_free(XokMalloc *xm, bytecode *bc);
int bc_expand(XokMalloc *xm, bytecode *bc);
int bc_emit(XokMalloc *xm, bytecode *bc, int instr);
int bc_add_constant(XokMalloc *xm, bytecode *bc, value v);
const char* typetostr(vartype type);
void assignencode(codesState cs, craastdesc *node, char *type);
#endif