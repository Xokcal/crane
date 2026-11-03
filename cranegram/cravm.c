#include <stdio.h>
#include "../tool/xokmalloc/xokmalloc.h"
#include "cracode.h"
#include "cravm.h"
#include "crafunc.h"
#include "cgm_gramma.h"
#include "cracom.h"
#include "craobject.h"

long vmhashcode(const char *s){
 long len = strlen(s);
 long r = 0;
 if(len > 5){
  for(int i=0;i<5;i++){
    r += (long)(int)s[i];
  }
  return (long)((r*31) % VAR_MAP_LEN);
 }else{
  for(int i=0;i<len;i++){
    r += (long)(int)s[i];
  }
  return (long)((r*31) % VAR_MAP_LEN);
 }
}

vmap *newvmap(XokMalloc *xm){
 vmap *map = (vmap*)xalloc_XokMalloc(xm, sizeof(vmap));
 map->count = 0;
 return map;
}

static inline vmapnode* newvmapnode(XokMalloc *xm,value v){
 vmapnode *node = (vmapnode*)xalloc_XokMalloc(xm, sizeof(vmapnode));
 node->v = v;
 node->next = NULL;
 return node;
}

void pushvmap(XokMalloc *xm, vmap *map, value v){
  long code = vmhashcode(v.vname);
  vmapnode *root = &map->map[code];
  vmapnode *new = newvmapnode(xm,v);
  if(root->v.vname == NULL){
    map->map[code] = *new;
    map->count++;
    return;
  }
  while(root->next != NULL){
    root = root->next; /* root.next is struct vmapnode*,
    after (*) is struct vmapnode */
  }
  root->next = new;
  map->count++;
  return;
}

value getvmap(vmap *map, const char *n){
 long code = vmhashcode(n);
 vmapnode *root = &map->map[code];
 while(root->v.vname != NULL){
  if(strcmp(root->v.vname, n)==0){
    return root->v;
  }
  root = root->next;
 }
 value empty;
 empty.vname = NULL;
 return empty;
}

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
    vm->ci++;
    return;
  }
 }
}


static value
find_value(vmState* vm, char *n, int *error)
{
 for(int i=0;i<vm->vsi;i++){
  value v = vm->vslots[i];
  if(!strcmp(n, v.vname))
   return v;
 }
 craCM_error(0,"var not have find!!");
 (*error) = 1;
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
opstore_executor(XokMalloc *xm, vmState *vm)
{
 value v; /* init value v, push stack later.*/
 vm->ci++;
 v.vname = strdup(get_codesv(vm));
 vm->ci = ((vm->ci) + 2);
 v.vtype = strdup(get_stack_type(vm));
 vm->ci++;
 v.v.i = get_stack_res(vm);
 v.type = TYPE_INT;
 pushvmap(xm, vm->map, v);
 //#define vmprint
 #ifdef vmprint
 value *r = vmvslots_pop(vm , v.vname);
 log(importancelogsign);
 logi("\n" , r->v.i);
 #endif
 clear_sp(vm);
}


/*#define storeundefexecutor(xm, vm)\
 value v;\
 v.type = TYPE_INT;\
 pushvmap((xm), (vm)->map, v);*/


static void
opcallexecute(vmState *vm, int *error)
{
 codenext(vm);
 char *callname = vm->tcalls->calls[getcode(vm)];
 if(craF_isbuildinfunc(callname)){
  if(!strcmp(callname, "print")){
   for(int i=0;i<vm->tempargsi;i++){
    if((*error)==1)return;
    Tparam tp = vm->tempargs[i];
    switch(tp.pty)
    {
     case PRM_STRING:prints(tp.originstr);
     break;
     case PRM_NUMBER:prmnumberP(tp,error);
     break;
     case PRM_VAR:prmvarP(tp, vm)
     break;
     default:break;
    }
   }
  }
 }
 println;
 vm->tempargsi = 0;
 codenext(vm);
}

void 
craV_execute(XokMalloc *xm , vmState *vm)
{
 vm->map = newvmap(xm);
 int error = 0;
 int *codes = vm->bc->codes;
 while(vm->ci < vm->bc->count){
  if(error == 1)
   goto vmterminal;
  vmswitch(getcode(vm)){
   vmcase(OP_CALL){
    opcallexecute(vm, &error);
    vmbreak;
   }
   vmcase(OP_LOAD){
    codenext(vm);
    opload_executor(vm);
    codenext(vm);
    vmbreak;
   }
   vmcase(OP_PUSH_VAR){
    codenext(vm);
    oppushvar_executor(vm, &error);
    codenext(vm);
    vmbreak;
   }
   vmcase(OP_PUSH){
    codenext(vm);
    oppush_executor(vm);
    vmbreak;
   }
   vmcase(OP_STORE){
    opstore_executor(xm, vm);
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
