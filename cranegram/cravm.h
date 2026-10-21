#ifndef cravm_h
#define cravm_h

#include <stdio.h>
#include "../tool/xokmalloc/xokmalloc.h"
#include "cracode.h"

#define calc_add(p1 , p2)(*(p1)) + (*(p2))
#define calc_sub(p1 , p2)(*(p1)) - (*(p2))
#define calc_mul(p1 , p2)(*(p1)) * (*(p2))
#define calc_divi(p1 , p2)(*(p1)) / (*(p2))

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
  vm.vs index */
  int sp;
  /* pc: struct bytecode
   bc codes index */
  int ci;
  /* vm vs stack, tackle 
  codes and parse */
  value *vs;
  int vni;
  int size;
}vm;

typedef struct vmmap{
  Hashmap *vmap; /* variable map store  */
}vmmap;


vm *vm_create(XokMalloc *xm, bytecode *bc, varnamestk *vns, int size);

void vm_free(XokMalloc *xm, vm *v);

/* vm main executor, use struct vm parse codes to stack 
** @param XokMaloc *xm
** @param vm *vm
*/
void craV_execute(XokMalloc *xm , Hashmap *vmap , vm *vm);

#endif