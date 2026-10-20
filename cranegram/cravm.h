#ifndef cravm_h
#define cravm_h

#include <stdio.h>
#include "../tool/xokmalloc/xokmalloc.h"
#include "cracode.h"

typedef struct bytecode bytecode;
typedef struct varnamestk varnamestk;
typedef struct value value;

typedef struct vm{
  /* struct bc: 
  codes and values package */
  bytecode *bc;
  /* struct vns: 
  var names package */
  varnamestk *vns;
  /* sp: struct 
  vm.stack index */
  int sp;
  /* pc: struct bytecode
   bc codes index */
  int pc;
  /* vm stack, tackle 
  codes and parse */
  value *vs;
  int vs_idx;
  int size;
}vm;

vm *vm_create(XokMalloc *xm, bytecode *bc, varnamestk *vns, int size);

void vm_free(XokMalloc *xm, vm *v);

/* vm main executor, use struct vm parse codes to stack 
** @param XokMaloc *xm
** @param vm *vm
*/
void craV_execute(XokMalloc *xm , vm *vm);

#endif