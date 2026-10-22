#include <stdio.h>
#include "../tool/xokmalloc/xokmalloc.h"
#include "cracode.h"
#include "cravm.h"

#define VM_DEFAULT_STACK_SIZE 1024
#define VM_DEFAULT_VSLOTS_SIZE 200

#define vmcase(op) case (op):
#define vmswitch(op) switch(op)
#define vmdefault default:
#define vmbreak break
#define dlong long long
#define voidunfind -1

void
vm_free(XokMalloc *xm, vm *v){
 if (!v) return; if (v->vs)v->vs = NULL;
}

/* instruct OP_PUSH execute */
#define oppush_executor(vm) \
 (vm)->vs[(vm)->sp++] = (vm)->bc->values[(vm)->bc->codes[(vm)->ci++]]

#define codes_idx(vm) \
 (vm)->bc->codes[(vm)->ci]

#define calc_swrap(res , resi , vm)\
 (vm)->vs[(vm)->sp - 2] = (res);\
 (vm)->sp = (vm)->sp - 1;

 /* virtual machine current ci point codes reflect struct value*/
#define get_codesv(vm)\
 (vm)->vns->vn[(vm)->bc->codes[(vm)->ci]]

 /* virtual machine only value after calculate*/
#define get_stack_res(vm)\
 (vm)->vs[(vm)->sp - 1].v.i

#define clear_sp(vm)\
 (vm)->sp = 0;

 /* virtual machine current ci point codes variable type */
#define get_stack_type(vm)\
 (vm)->tys->types[(vm)->bc->codes[(vm)->ci]]

/* virtual machine vslots push value*/
#define vmvslots_push(vm , v)\
 (vm)->vslots[(vm)->vsi++] = (v);

vm *
vm_create(XokMalloc *xm, bytecode *bc, varnamestk *vns,
                               Ttypes *tys, int size){
 if (size <= 0) size = VM_DEFAULT_STACK_SIZE;
 vm *v = (vm *)xalloc_XokMalloc(xm, sizeof(vm));
 if (!v) return NULL;
 v->vs = (value *)xalloc_XokMalloc
 (xm, sizeof(value) * size);
 v->vslots = (value *)xalloc_XokMalloc
 (xm, sizeof(value) * VM_DEFAULT_VSLOTS_SIZE);
 if (!v->vs) return NULL;
 v->bc = bc;
 v->vns = vns;
 v->tys = tys;
 v->sp = 0;
 v->ci = 0;
 v->size = size;
 v->vni = 0;
 v->vsi = 0;
 return v;
}


static dlong
calc_executor(dlong *p1 , dlong *p2 , char *sign)
{
 if(strcmp(sign , "+")==0)
  return (dlong)calc_add(p1 , p2);
 if(strcmp(sign , "-")==0)
  return (dlong)calc_sub(p1 , p2);
 if(strcmp(sign , "*")==0)
  return (dlong)calc_mul(p1 , p2);
 if(strcmp(sign , "/")==0){
  if(*p2 == 0){cra_error("/ 0");return 0;}
  return (dlong)calc_divi(p1 , p2);
 }
 return 0;
}


static void
opcalc_executor(vm *vm , char *sign){
 value res;
 dlong resi;
 value value1 = vm->vs[vm->sp - 2];
 value value2 = vm->vs[vm->sp - 1];
 vmswitch(value1.type){
  vmcase(TYPE_INT){
   dlong v1 = (dlong)value1.v.i;
   dlong v2 = (dlong)value2.v.i;
   log_i("v1" , v1);
   log_i("v2" , v2);
   resi = calc_executor(&v1 , &v2 , sign);
   res.type = TYPE_INT;
   res.v.i = (int)resi;
   log_i("opadd_result" , res.v.i);
   calc_swrap(res , resi , vm);
   vm->ci++;
   return;
  }
  vmdefault{
    return;
  }
 }
}

inline value *
vmvslots_pop(vm *vm , char *n)
{
 int vsitemp = 0;
 while(vsitemp < vm->vsi){
  if(strcmp(vm->vslots[vsitemp].vname , n)==0)
   return &(vm->vslots[vsitemp]);
  vsitemp++;
 }
 return NULL;
}


void 
craV_execute(XokMalloc *xm , vm *vm)
{
 log_c("varName" , vm->vns->vn[0]);
 int *codes = vm->bc->codes;
 while(vm->ci < vm->bc->count){
  log_i("loop codes_idx:" , codes_idx(vm));
  vmswitch(codes_idx(vm)){
   vmcase(OP_PUSH){
    log("___OP_PUSH");
    log_i("" , vm->bc->values[0].v.i);
    log_i("pc.value.v" , (vm)->bc->values[(vm)->ci].v.i);
    vm->ci++;
    oppush_executor(vm);
    for(int i = 0; i < vm->sp; i++)
     log_i("vm.vs.i",vm->vs[i].v.i);
    log_i("en PC:" , vm->ci);
    log_i("en vs_idx:" , vm->sp);
    vmbreak;
   }
   vmcase(OP_STORE){
    value v;
    vm->ci++;
    char *vname = get_codesv(vm);
    log_c("vname+++" , vname);
    vm->ci++;
    char *type = get_stack_type(vm);
    log_c("type+++" , type);
    log_i("res******" , get_stack_res(vm));
    vm->ci++;
    v.vtype = strdup(type);
    v.vname = strdup(vname);
    v.v.i = get_stack_res(vm);
    {
     log("{");
     logi("v" , v.v);
     logc("vname" , v.vname);
     logc("vtype" , v.vtype);
     log("}");
    }
    vmvslots_push(vm , v);
    log("_________________________");
    value *r = vmvslots_pop(vm , vname);
    logi("result b" , r->v.i);
    clear_sp(vm);
    vmbreak;
   }
   vmcase(OP_ADD){
    opcalc_executor(vm , "+");
    vmbreak;
   }
   vmcase(OP_SUB){
    opcalc_executor(vm , "-");
    vmbreak;
   }
   vmdefault{
    vm->ci++;
    vmbreak;
   }
  }
 }
}
