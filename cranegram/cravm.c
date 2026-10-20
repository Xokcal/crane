#include <stdio.h>
#include "../tool/xokmalloc/xokmalloc.h"
#include "cracode.h"
#include "cravm.h"

#define VM_DEFAULT_STACK_SIZE 1024
#define vmcase(op) case (op):
#define vmswitch(op) switch(op)
#define vmdefault default:
#define vmbreak break

void
vm_free(XokMalloc *xm, vm *v){
 if (!v) return; if (v->vs)v->vs = NULL;
}

#define oppush_executor(vm) \
 (vm)->vs[(vm)->sp++] = (vm)->bc->values[(vm)->vs_idx++]


vm *
vm_create(XokMalloc *xm, bytecode *bc, varnamestk *vns, int size)
{
 if (size <= 0) size = VM_DEFAULT_STACK_SIZE;
 vm *v = (vm *)xalloc_XokMalloc(xm, sizeof(vm));
 if (!v) return NULL;
 v->vs = (value *)xalloc_XokMalloc(xm, sizeof(value) * size);
 if (!v->vs) return NULL;
 v->bc = bc;
 v->vns = vns;
 v->sp = 0;
 v->pc = 0;
 v->size = size;
 v->vs_idx = 0;
 return v;
}


void
craV_execute(XokMalloc *xm , vm *vm)
{
 LOG_C("varName" , vm->vns->vn[0]);
 int *codes = vm->bc->codes;
 while(vm->sp < vm->bc->count){
  LOG_I("cidx" , vm->sp);
  int op = vm->bc->codes[vm->pc++];
  vmswitch(op){
    vmcase(OP_PUSH){
      LOG("OP_PUSH");
      oppush_executor(vm);
      LOG_I("vm.vs.i",vm->vs[0].v.i);
      vm->pc++;
      vmbreak;
    }
    vmcase(OP_ADD){
      int add;
      vm->sp++;
      vmbreak;
    }
    vmdefault{
      vm->sp++;
    }
  }
 }
}
