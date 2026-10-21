#include <stdio.h>
#include <string.h>
#include "cracode.h"
#include "../tool/xokmalloc/xokmalloc.h"
#include "craparse.h"
#include "cgm_parse.h"
#include "cravm.h"

#define BYTECODE_DEFAULT_SIZE 16
#define BYTECODE_GROW_FACTOR  2
#define craC_codes_log(bc) \
  for(int i = 0; i < (bc)->count; i++) LOG_I("BC" , (bc)->codes[i]);

#define BC_DEFAULT_SIZE 16


/* void *vv is value , vm by char *type judge 
var type and push stack*/
static void
v_type_insert(char *type , value *v , void *vv)
{
  if(strcmp(type , "int")==0){
    v->type = TYPE_INT;
    v->v.i = *(int*)vv;
  }
  if(strcmp(type , "float")==0){
    v->type = TYPE_FLOAT;
    v->v.f = *(float*)vv;
  }
  if(strcmp(type , "string")==0){
    v->type = TYPE_STRING;
    /* remember freev.v.s(string) */
    v->v.s = strdup((char*)vv);
  }
  if(strcmp(type , "long")==0){
    v->type = TYPE_LONG;
    v->v.l = *(long*)vv;
  }
  if(strcmp(type , "char")==0){
    v->type = TYPE_CHAR;
    v->v.c = *(char*)vv;
  }
}


bytecode *
bc_create(XokMalloc *xm, int init_size)
{
  if (init_size <= 0) init_size = BC_DEFAULT_SIZE;
  bytecode *bc = (bytecode *)xalloc_XokMalloc(xm, sizeof(bytecode));
  if (!bc) return NULL;
  bc->codes = (int *)xalloc_XokMalloc(xm, sizeof(int) * init_size);
  bc->values = (value *)xalloc_XokMalloc(xm, sizeof(value) * init_size);
  if (!bc->codes || !bc->values)return NULL;
  bc->count = 0;
  bc->size  = init_size;
  bc->v_count = 0;
  bc->v_size  = init_size;
  return bc;
}


void
bc_free(XokMalloc *xm, bytecode *bc)
{
  if (!bc) return;
  if (bc->values) {
    for (int i = 0; i < bc->v_count; i++) {
      if (bc->values[i].type == TYPE_STRING)
        {free(bc->values[i].v.s);}
    }
  }
  if (bc->codes){}
}


int
bc_expand(XokMalloc *xm, bytecode *bc)
{
  if (!bc) return 0;
  int new_size = bc->size * 2;
  int *new_codes = (int *)xalloc_XokMalloc(xm, sizeof(int) * new_size);
  if (!new_codes) return 0;
  memcpy(new_codes, bc->codes, sizeof(int) * bc->count);
  bc->codes = new_codes;
  bc->size  = new_size;
  value *new_values = (value *)xalloc_XokMalloc(xm, sizeof(value) * new_size);
  if (!new_values) return 0;
   memcpy(new_values, bc->values, sizeof(value) * bc->v_count);
  bc->values = new_values;
  bc->v_size = new_size;
  return 1;
}


int
bc_emit(XokMalloc *xm, bytecode *bc, int instr)
{
  if (!bc) return 0;
  if (bc->count >= bc->size)
      if (!bc_expand(xm, bc)) return 0;
  bc->codes[bc->count++] = instr;
  return 1;
}


int
bc_add_constant(XokMalloc *xm, bytecode *bc, value v)
{
  if (!bc) return -1;
  if (bc->v_count >= bc->v_size)
      if (!bc_expand(xm, bc)) return -1;
  bc->values[bc->v_count] = v;
  return bc->v_count++;
}


static varnamestk *
vnstk_create(XokMalloc *xm, int init_size)
{
  if (init_size <= 0) init_size = 16;
  varnamestk *vs = (varnamestk *)xalloc_XokMalloc(xm, sizeof(varnamestk));
  if (!vs) return NULL;
  vs->vn    = (char **)xalloc_XokMalloc(xm, sizeof(char *) * init_size);
  vs->slots = (int *)xalloc_XokMalloc(xm, sizeof(int) * init_size);
  if (!vs->vn || !vs->slots) return NULL;
  vs->size  = init_size;
  vs->count = 0;
  return vs;
}

static void
vnstk_free(XokMalloc *xm, varnamestk *vs)
{
  if (!vs) return;
}

static int
vnstk_expand(XokMalloc *xm, varnamestk *vs)
{
  if (!vs) return 0;
  int new_size = vs->size * 2;
  char **new_vn = (char **)xalloc_XokMalloc(xm, sizeof(char *) * new_size);
  int   *new_slots = (int *)xalloc_XokMalloc(xm, sizeof(int) * new_size);
  if (!new_vn || !new_slots) return 0;
  memcpy(new_vn, vs->vn, sizeof(char *) * vs->count);
  memcpy(new_slots, vs->slots, sizeof(int) * vs->count);
  vs->vn    = new_vn;
  vs->slots = new_slots;
  vs->size  = new_size;
  return 1;
}

static int
vnstk_push(XokMalloc *xm, varnamestk *vs, const char *name, int slot)
{
  if (!vs) return 0;
  if (vs->count >= vs->size) {
      if (!vnstk_expand(xm, vs)) return 0;
  }
  vs->vn[vs->count]    = strdup(name);
  vs->slots[vs->count] = slot;
  vs->count++;
  return 1;
}

static int
vnstk_find(varnamestk *vs, const char *name)
{
  for (int i = 0; i < vs->count; i++) {
    if (strcmp(vs->vn[i], name) == 0)return vs->slots[i];
  }
  return -1;
}


static int
sign_emit(XokMalloc *xm , char *s , bytecode *bc)
{
  if(strcmp(s , "+")==0)return bc_emit(xm , bc , OP_ADD);
  if(strcmp(s , "-")==0)return bc_emit(xm , bc , OP_SUB);
  if(strcmp(s , "*")==0)return bc_emit(xm , bc , OP_MUL);
  if(strcmp(s , "/")==0)return bc_emit(xm , bc , OP_DIV);
}


static void
assign_encode(XokMalloc *xm, craastdesc *node ,
bytecode *bc , char *type)
{
  if (node->kind == AST_NUM) { /* single tackle node
    is AST_NUM, if is binop node don't push*/
    LOG("encode en num ADD!!!!");
    value v;
    LOG_I("num" , node->as.num);
    v_type_insert(type , &v , &node->as.num);
    LOG_I("v" , v.v.i);
    bc_emit(xm, bc, OP_PUSH);
    bc_emit(xm , bc , bc->v_count);
    bc_add_constant(xm, bc , v);
    return;
  }
  if (node->kind == AST_BINOP) { /* left-right-mid recursion，
    leftest is highest priority binop, priority tackle leftest binop
    left-num and right-num, rightest is low priority, can tabckle later */
    assign_encode(xm, node->as.binop.left, bc , type);
    assign_encode(xm, node->as.binop.right, bc , type);
    sign_emit(xm, node->as.binop.calcsign, bc);
    return;
  }
  return;
}


static bytecode*
assign_code(XokMalloc *xm , craastdesc *desc , bytecode *bc)
{
  craastdesc *value = desc->as.assign.value;
  char *type = desc->as.assign.type;
  assign_encode(xm , value , bc , type);
  return bc;
}


vm*
craC_bytecode_parse(XokMalloc *xm ,  desclist *dl)
{
  int dl_c = 0;
  bytecode *bc = bc_create(xm , BYTECODE_DEFAULT_SIZE);
  varnamestk *vns = vnstk_create(xm , 16);
  while(dl_c < dl->count){
    craastdesc *curdesc = dl->desclist[dl_c];
    switch(curdesc->kind){
      case AST_ASSIGN:{
        vnstk_push(xm , vns , curdesc->as.assign.name , dl_c);
        assign_code(xm , curdesc , bc);
        bc_emit(xm , bc , OP_STORE);
        bc_emit(xm , bc , dl_c);
        LOG_I("bc.values.v[1]" , bc->values[0].v.i);
        LOG_I("bc.values.v[2]" , bc->values[1].v.i);
        dl_c++;
        break;
      }
      case AST_EMPTY:{
        break;
      }
      default:{
        break;
      }
    }
  }
  bc_emit(xm , bc , OP_HALT);
  craC_codes_log(bc);
  return vm_create(xm , bc , vns , 160);
}