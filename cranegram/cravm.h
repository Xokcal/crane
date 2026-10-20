#ifndef cravm_h
#define cravm_h

#include <stdio.h>
#include "../tool/xokmalloc/xokmalloc.h"
#include "cracode.h"

typedef struct bytecode bytecode;
typedef struct varnamestk varnamestk;

typedef struct vm{
  bytecode *bc;
  varnamestk *vns;
  int sp;
  int pc;
  int *stack;
  int size;
}vm;

vm *vm_create(XokMalloc *xm, bytecode *bc, varnamestk *vns, int size);

void vm_free(XokMalloc *xm, vm *v);

void craV_execute(XokMalloc *xm , vm *vm);

#endif