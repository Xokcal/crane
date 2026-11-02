#ifndef cravm_h
#define cravm_h

#include <stdio.h>
#include "../tool/xokmalloc/xokmalloc.h"
#include "cracode.h"
#include "craparse.h"
#include "craobject.h"

typedef struct bytecode bytecode;
typedef struct varnamestk varnamestk;
typedef struct Ttypes Ttypes;
typedef struct Tparam Tparam;
typedef struct Targs Targs;
typedef struct Tcalls Tcalls;

#define calc_add(p1 , p2)(*(p1)) + (*(p2))
#define calc_sub(p1 , p2)(*(p1)) - (*(p2))
#define calc_mul(p1 , p2)(*(p1)) * (*(p2))
#define calc_divi(p1 , p2)(*(p1)) / (*(p2))

#define VAR_MAP_LEN 108

#define VM_DEFAULT_STACK_SIZE 3000000
#define VM_DEFAULT_VSLOTS_SIZE 3000000
#define VM_DEFAULT_TEMPARGS_SIZE 3000000
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

/* ************ virtual machine ************* */
/* instruct OP_PUSH execute */
#define oppush_executor(vm) \
 (vm)->vs[(vm)->sp++] = (vm)->bc->values[(vm)->bc->codes[(vm)->ci++]]

 /* instruct OP_PUSH_VAR execute */
#define oppushvar_executor(vm, error) \
 (vm)->vs[(vm)->sp++] = getvmap((vm)->map, (vm)->vns->vn[getcode((vm))])

/* current codes's bytecode */
#define getcode(vm) \
 (vm)->bc->codes[(vm)->ci]

/* instruct OP_LOAD execute */
#define opload_executor(vm)\
 (vm)->tempargs[(vm)->tempargsi++] = (vm)->targs->args[getcode((vm))]

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

#define strfree(s) string_free((s))


/* ************** define function **************** */
#define prmnumberP(tp, error) \
 String *number = create_string((tp).originstr); \
 int *r = str_convert_int(number); \
 printi(*r); \
 free(r); \
 strfree(number);

 #define prmvarP(tp, vm) \
 value v = getvmap((vm)->map, (tp).originstr);\
 printi(v.v.i);


typedef struct vmapnode{
 struct vmapnode *next;
 value v;
}vmapnode;

typedef struct vmap{
 vmapnode map[108];
 long count;
}vmap;

typedef struct vmState{
 vmap *map;
 Tcalls *tcalls; /* call name stack */
 Targs *targs; /* codes parse store all targs */
 /* struct bc: 
 codes and values package */
 bytecode *bc;
 /* struct vns: 
 var names package */
 varnamestk *vns;
 /* struct types: 
 var type package */
 Ttypes *tys;
 /* sp: struct 
 vm.vs index */
 int sp;
 /* pc: struct bytecode
  bc codes index */
 int ci;
 /* vm vs stack, tackle 
 codes and parse */
 value *vs;
 /* variable struct value slots
 use to search variable value */
 value *vslots;
 /* value vslots variable index */
 int vsi;
 int vni;
 Tparam *tempargs; /* temp args stack */
 int tempargsi; /* temp args stack point */
 int size;
}vmState;

typedef struct vmmap{
  Hashmap *vmap; /* variable map store  */
}vmmap;

long vmhashcode(const char *s);
vmap *newvmap(XokMalloc *xm);
void pushvmap(XokMalloc *xm, vmap *map, value v);
value getvmap(vmap *map, const char *n);
vmState *vm_create(XokMalloc *xm, bytecode *bc, varnamestk *vns,
  Ttypes *tys, Targs *Targs, Tcalls *tcalls, int size);
void vm_free(XokMalloc *xm, vmState *v);
/* vmState main executor, use struct vm parse codes to stack */
void craV_execute(XokMalloc *xm , vmState *vm);
value* vmvslots_pop(vmState *vm , char *n);

//#define vmgslotsv(vm , n , ty , T) (T*)vmvslots_popv((vm), (n), (ty));

#endif