#include <stdio.h>
#include "../tool/xokmalloc/xokmalloc.h"
#include "cracode.h"
#include "cravm.h"
#include "crafunc.h"
#include "cgm_gramma.h"
#include "cracom.h"

#define VM_DEFAULT_STACK_SIZE 1024
#define VM_DEFAULT_VSLOTS_SIZE 300
#define VM_DEFAULT_TEMPARGS_SIZE 300
#define importancelogsign "________________________"
#define addsign "+"
#define subsign "-"
#define mulsign "*"
#define divsign "/"

#define vmcase(op) case (op):
#define vmswitch(op) switch(op)
#define vmdefault default:
#define vmbreak break
#define dlong long long
#define voidunfind -1

/* ************ build-in function *********** */
#define prints(s) printf("%s", (s))
#define printi(i) printf("%d",(i))
#define println printf("\n")

/* instruct OP_PUSH execute */
#define oppush_executor(vm) \
 (vm)->vs[(vm)->sp++] = (vm)->bc->values[(vm)->bc->codes[(vm)->ci++]]

/* instruct OP_LOAD execute */
#define opload_executor(vm)\
 (vm)->tempargs[(vm)->tempargsi++] = (vm)->targs->args[getcode((vm))]

 /* current codes's bytecode */
#define getcode(vm) \
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

/* ********** virtual machine *********** */
#define codenext(vm)\
 (vm)->ci++;

/* ************** String func tackle ***************** */
#define cmptail(s , tail) combine_tail((s), (tail))

#define strclear(s) delete_all((s));

#define strfree(s) string_free((s))


inline void vm_free(XokMalloc *xm, vmState *v){
 if (!v) return;
  if (v->vs) {
   free(v->vs); 
   v->vs = NULL;
  }
  if (v->vslots) {
   for (int i = 0; i < v->vsi; i++) {
       if (v->vslots[i].vname) free(v->vslots[i].vname);
       if (v->vslots[i].vtype) free(v->vslots[i].vtype);
   }
   free(v->vslots);
   v->vslots = NULL;
  }
   free(v);
}

vmState *
vm_create(XokMalloc *xm, bytecode *bc, varnamestk *vns,
  Ttypes *tys, Targs *Targs, Tcalls *tcalls, int size){
 if (size <= 0) size = VM_DEFAULT_STACK_SIZE;
 vmState *v = (vmState *)xalloc_XokMalloc(xm, sizeof(vmState));
 if (!v) return NULL;
 v->vs = (value *)xalloc_XokMalloc
 (xm, sizeof(value) * size);
 v->vslots = (value *)xalloc_XokMalloc
 (xm, sizeof(value) * VM_DEFAULT_VSLOTS_SIZE);
 v->tempargs = (Tparam*)
 xalloc_XokMalloc(xm, sizeof(Tparam) * VM_DEFAULT_TEMPARGS_SIZE);
 if (!v->vs) return NULL;
 v->bc = bc;
 v->vns = vns;
 v->tys = tys;
 v->targs = Targs;
 v->tempargsi = 0;
 v->tcalls = tcalls;
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
opcalc_executor(vmState *vm , char *sign){
 value res;
 dlong resi;
 value value1 = vm->vs[vm->sp - 2];
 value value2 = vm->vs[vm->sp - 1];
 vmswitch(value1.type){
  vmcase(TYPE_INT){
   dlong v1 = (dlong)value1.v.i;
   dlong v2 = (dlong)value2.v.i;
   //#define opaddv1v2
   #ifdef opaddv1v2
   log_i("v1" , v1);
   log_i("v2" , v2);
   #endif
   resi = calc_executor(&v1 , &v2 , sign);
   res.type = TYPE_INT;
   res.v.i = (int)resi;
   //#define opaddr
   #ifdef opaddr
   log_i("opadd_result" , res.v.i);
   #endif
   calc_swrap(res , resi , vm);
   vm->ci++;
   return;
  }
  vmdefault{
    return;
  }
 }
}


static value
find_value(vmState* vm, char *n)
{
 for(int i=0;i<vm->vsi;i++){
  value v = vm->vslots[i];
  if(!strcmp(n, v.vname))
   return v;
 }
 craCM_error("var not have find!!");
}


inline value *
vmvslots_pop(vmState *vm , char *n)
{
 int vsitemp = 0;
 while(vsitemp < vm->vsi){
  if(strcmp(vm->vslots[vsitemp].vname , n)==0)
   return &(vm->vslots[vsitemp]);
  vsitemp++;
 }
 return NULL;
}


static void
opstore_executor(vmState *vm)
{
 value v; /* init value v, push stack later.*/
 vm->ci++; /*  */
 v.vname = strdup(get_codesv(vm));
 vm->ci++;
 v.vtype = strdup(get_stack_type(vm));
 vm->ci++;
 v.v.i = get_stack_res(vm);
 vmvslots_push(vm , v);
 value *r = vmvslots_pop(vm , v.vname);
 //#define vmprint
 #ifdef vmprint
  log(importancelogsign);
  logi("\n" , r->v.i);
 #endif
 clear_sp(vm);
}


static void
buildin_func_executor()
{
 
}


void 
craV_execute(XokMalloc *xm , vmState *vm)
{
 int *codes = vm->bc->codes;
 while(vm->ci < vm->bc->count){
  vmswitch(getcode(vm)){
   vmcase(OP_CALL){
    codenext(vm);
    char *callname = vm->tcalls->calls[getcode(vm)];
    if(craF_isbuildinfunc(callname)){
     if(!strcmp(callname, "print")){
      for(int i=0;i<vm->tempargsi;i++){
       Tparam tp = vm->tempargs[i];
       switch(tp.pty)
       {
        case PRM_STRING:prints(tp.originstr);break;
        case PRM_NUMBER:
        String *number = create_string(tp.originstr);
        int *r = str_convert_int(number);
        printi(*r);
        free(r);
        strfree(number);
        break;
        case PRM_VAR:
        value v = find_value(vm, tp.originstr);
        printi(v.v.i);
        break;
        default:break;
       }
      }
     }
    }
    println;
    vm->tempargsi = 0;
    codenext(vm);
    vmbreak;
   }
   vmcase(OP_LOAD){
    codenext(vm);
    opload_executor(vm);
    codenext(vm);
    vmbreak;
   }
   vmcase(OP_PUSH){
    codenext(vm);
    oppush_executor(vm);
    vmbreak;
   }
   vmcase(OP_STORE){
    opstore_executor(vm);
    vmbreak;
   }
   vmcase(OP_ADD){
    opcalc_executor(vm , addsign);
    vmbreak;
   }
   vmcase(OP_SUB){
    opcalc_executor(vm , subsign);
    vmbreak;
   }
   vmcase(OP_MUL){
    opcalc_executor(vm , mulsign);
    vmbreak;
   }
   vmcase(OP_DIV){
    opcalc_executor(vm , divsign);
    vmbreak;
   }
   vmcase(OP_HALT){
    goto vmterminal;
   }
   vmdefault{
    codenext(vm);
    vmbreak;
   }
  }
 }
 vmterminal:
}
