#include <stdio.h>
#include "../tool/xokmalloc/xokmalloc.h"
#include "cracode.h"
#include "cravm.h"

#define VM_DEFAULT_STACK_SIZE 1024
#define vmcase(op) case (op):
#define vmswitch(op) switch(op)
#define vmdefault default:
#define vmbreak break
#define dlong long long

void
vm_free(XokMalloc *xm, vm *v){
 if (!v) return; if (v->vs)v->vs = NULL;
}

#define oppush_executor(vm) \
 (vm)->vs[(vm)->sp++] = (vm)->bc->values[(vm)->bc->codes[(vm)->ci++]]

#define codes_idx(vm) \
 (vm)->bc->codes[(vm)->ci]

#define calc_swrap(res , resi , vm)\
 (vm)->vs[(vm)->sp - 2] = (res);\
 (vm)->sp = (vm)->sp - 1;

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
 v->ci = 0;
 v->size = size;
 v->vsi = 0;
 return v;
}


static dlong
calc_executor(dlong *p1 , dlong *p2 , char *sign)
{
 if(strcmp(sign , "+")==0){
  return (dlong)calc_add(p1 , p2);
 }
 if(strcmp(sign , "+")==0)
  return (dlong)calc_sub(p1 , p2);
 if(strcmp(sign , "+")==0)
  return (dlong)calc_mul(p1 , p2);
 if(strcmp(sign , "+")==0){
  if(*p2 == 0){cra_error("/ 0");return 0;}
  return (dlong)calc_divi(p1 , p2);
 }
 return 0;
}


static void
opadd_executor(vm *vm , char *sign){
 value res;
 dlong resi;
 value value1 = vm->vs[vm->sp - 2];
 value value2 = vm->vs[vm->sp - 1];
 vmswitch(value1.type){
  vmcase(TYPE_INT){
   dlong v1 = (dlong)value1.v.i;
   dlong v2 = (dlong)value2.v.i;
   LOG_I("v1" , v1);
   LOG_I("v2" , v2);
   resi = calc_executor(&v1 , &v2 , sign);
   res.type = TYPE_INT;
   res.v.i = (int)resi;
   LOG_I("opadd_result" , res.v.i);
   calc_swrap(res , resi , vm);
   return;
  }
  vmdefault{
    return;
  }
 }
}

void
craV_execute(XokMalloc *xm , vm *vm)
{
 LOG_C("varName" , vm->vns->vn[0]);
 int *codes = vm->bc->codes;
 while(vm->ci < vm->bc->count){
  LOG_I("loop codes_idx:" , codes_idx(vm));
  vmswitch(codes_idx(vm)){
   vmcase(OP_PUSH){
    LOG("___OP_PUSH");
    LOG_I("" , vm->bc->values[0].v.i);
    LOG_I("pc.value.v" , (vm)->bc->values[(vm)->ci].v.i);
    vm->ci++;
    oppush_executor(vm);
    for(int i = 0; i < vm->sp; i++)
     LOG_I("vm.vs.i",vm->vs[i].v.i);
    LOG_I("en PC:" , vm->ci);
    LOG_I("en vs_idx:" , vm->sp);
    vmbreak;
   }
   vmcase(OP_ADD){
    opadd_executor(vm , "+");
    vm->ci++;
    LOG_I("_________result" , vm->vs[vm->sp - 1].v.i);
    vmbreak;
   }
   vmcase(OP_STORE){
    
    vmbreak;
   }
   vmdefault{
    vm->ci++;
    vmbreak;
   }
  }
 }
}
