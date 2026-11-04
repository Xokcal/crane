#include <stdio.h>
#include "../tool/xokmalloc/xokmalloc.h"
#include "craparse.h"

/*
********************************************************
                    AST TREE CREATE
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