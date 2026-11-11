#include <stdio.h>
#include "../tool/xokmalloc/xokmalloc.h"
#include "craparse.h"

/*
********************************************************
*                   AST TREE CREATE                    *
********************************************************
*/

/*
** ast parse variable string name to struct desc
*/
craastdesc *make_var(XokMalloc *xokmalloc , char *name){
 craastdesc *node = (craastdesc *)
 xalloc_XokMalloc(xokmalloc , sizeof(craastdesc));
 node->kind = AST_VAR;
 node->as.varname = strdup(name);  /* remember free node name */
 return node;
}

craastdesc* make_num(XokMalloc *xokMalloc ,  int n){
 craastdesc *node = (craastdesc *)
 xalloc_XokMalloc(xokMalloc , sizeof(craastdesc));
 node->kind = AST_NUM;
 node->as.num = n;  /* store number value */
 return node;
}

craastdesc *make_binop(XokMalloc *xokMalloc, char *calcsign,
                      craastdesc *left , craastdesc *right){
 craastdesc *node = (craastdesc *)
 xalloc_XokMalloc(xokMalloc , sizeof(craastdesc));
 node->kind = AST_BINOP;
 node->as.binop.calcsign = strdup(calcsign); /* remember free node.left! */
 node->as.binop.left = left; 
 node->as.binop.right = right;
 return node;
}

craastdesc *make_assign(XokMalloc *xokMalloc, 
       char *name , craastdesc *value , char *type){
 craastdesc *node = (craastdesc *)
 xalloc_XokMalloc(xokMalloc , sizeof(craastdesc));
 node->kind = AST_ASSIGN;
 node->as.assign.name = strdup(name);
 node->as.assign.value = value;
 node->as.assign.type = strdup(type);
 return node;
}

craastdesc *make_call(XokMalloc *xokMalloc, char *name){
 craastdesc *node = (craastdesc *)
 xalloc_XokMalloc(xokMalloc , sizeof(craastdesc));
 node->as.call.args = (Tparam *)
 xalloc_XokMalloc(xokMalloc , sizeof(Tparam) * makefnTpysize);
 node->kind = AST_CALL;
 node->as.call.fname = strdup(name);
 node->as.call.argsi = 0;
 return node;
}

craastdesc *make_lvalue(XokMalloc *xokmalloc , char *name){
 craastdesc *node = (craastdesc *)
 xalloc_XokMalloc(xokmalloc , sizeof(craastdesc));
 node->kind = AST_LVNAME;
 node->as.lvname = strdup(name);  /* remember free node name */
 return node;
}

craastdesc *make_hasvar(XokMalloc *xokmalloc
      , craastdesc *value, char *name, char *type){
 craastdesc *node = (craastdesc *)
 xalloc_XokMalloc(xokmalloc , sizeof(craastdesc));
 node->kind = AST_HASVAR;
 node->as.hasvar.name = strdup(name);
 node->as.hasvar.value = value;
 node->as.hasvar.type = strdup(type);
 return node;
}

craastdesc *make_cmp(XokMalloc *xokmalloc, char *cmpc,
                craastdesc *left , craastdesc *right){
 craastdesc *node = (craastdesc *)
 xalloc_XokMalloc(xokmalloc , sizeof(craastdesc));
 node->kind = AST_CMP;
 node->as.binop.calcsign = strdup(cmpc);
 node->as.binop.left = left;
 node->as.binop.right = right;
 return node;
}


craastdesc *make_condi(XokMalloc *xokmalloc, char *cmpc,
                craastdesc *left , craastdesc *right){
 craastdesc *node = (craastdesc *)
 xalloc_XokMalloc(xokmalloc , sizeof(craastdesc));
 node->kind = AST_CONDI;
 node->as.binop.calcsign = strdup(cmpc);
 node->as.binop.left = left;
 node->as.binop.right = right;
 return node;
}


craastdesc *make_cbdesc(XokMalloc *xokmalloc, craastdesc *condi,
                               Tblock *block, craasttype type){
 craastdesc *node = (craastdesc *)
 xalloc_XokMalloc(xokmalloc , sizeof(craastdesc));
 if(type == GRAM_IF) node->kind = AST_IF;
 else if(type == GRAM_WHILE) node->kind = AST_WHILE;
 node->as.ifdesc.condi = condi;
 node->as.ifdesc.block = block;
 return node;
}

/*
**********************************************************
*                  DescList factor                       *
**********************************************************
*/

/*
** Create a description list with initial capacity 'init_size'.
** A non-positive 'init_size' uses DESCLIST_DEFAULT_SIZE.
** Returns NULL if allocation fails.
*/
desclist *craP_desclist_create(XokMalloc *xm, int init_size){
  if (init_size <= 0) init_size = DESCLIST_DEFAULT_SIZE;
  desclist *dl = (desclist *)xalloc_XokMalloc(xm, sizeof(desclist));
  if (!dl) return NULL;
  dl->desclist = (craastdesc **)xalloc_XokMalloc(
  xm, sizeof(craastdesc *) * init_size);
  if (!dl->desclist)return NULL; /* dl non-create return NULL */
  dl->size  = init_size;
  dl->count = 0;
  return dl;
}


/* desclist free let desc arrays equal NULL, dl use xokmallc pool need't free */
void craP_desclist_free(XokMalloc *xm, desclist *dl){
  if (!dl) return;
  if (dl->desclist)dl->desclist = NULL;
  dl->count = 0;
  dl->size = 0;
}


/*
** New desc tree push desclist stack, if dl count >= dl size
** use desclist expand funtion.
*/
int craP_desclist_push(XokMalloc *xm, desclist *dl, craastdesc *tree){
  if (!dl) return 0;
  if (dl->count >= dl->size) {
  if (!craP_desclist_expand(xm, dl))return 0;}
  dl->desclist[dl->count++] = tree;
  return 1;
}


/*
** Desc list expand size by (origin size * DESCLIST_GROW_FACTOR).
** memcpy function copy dl's origin desclist and give to new desclist.
** need't free origin desclist, it use xokmalloc pool. 
*/
int craP_desclist_expand(XokMalloc *xm, desclist *dl){
  if (!dl) return 0;
  int new_size = dl->size * DESCLIST_GROW_FACTOR;
  craastdesc **new_arr = (craastdesc **)xalloc_XokMalloc
  (xm, sizeof(craastdesc *) * new_size);
  if (!new_arr) return 0; /* non-create return 0. */
  memcpy(new_arr, dl->desclist, sizeof(craastdesc *) * dl->count);
  dl->desclist = new_arr;
  dl->size = new_size;
  return 1; /* success return 1. */
}


/*
*****************************************************
*                Tblock factor                      *
*****************************************************
*/
Tblock *newTblock(XokMalloc *xm){
 Tblock *block = (Tblock*)xalloc_XokMalloc(xm, sizeof(Tblock));
 block->treelist = craP_desclist_create(xm, 16);
 block->count = 0;
 return block;
}

/*
******************************************************
*                 Istack factor                      *
******************************************************
*/
Istack* newIstack(XokMalloc *xm){
  Istack *stack = (Istack*)xalloc_XokMalloc(xm, sizeof(Istack));
  stack->count = 0;
  stack->p = 0;
  return stack;
}

void pushI(Istack *stack, int i){
  stack->stack[stack->p++] = i;
  stack->count++;
}

int popI(Istack *stack){
 if(stack->stack == 0)return 0;
 stack->count--;
 return stack->stack[--stack->p];
}