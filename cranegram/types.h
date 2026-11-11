#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>
#include "../tool/String/String.h"
#include "cralex.h"
#include "../tool/map/hashmap_int.h"
#include "../tool/xokmalloc/xokmalloc.h"
#include "craparse.h"

typedef struct craastdesc craastdesc;
typedef struct Tblock Tblock;
typedef struct desclist desclist;
typedef enum craasttype craasttype;

typedef struct Istack{
 int stack[200];
 int p;
 int count;
}Istack;


/* make desc func */
craastdesc *make_var(XokMalloc *xokmalloc , char *name);
craastdesc* make_num(XokMalloc *xokMalloc ,  int n);
craastdesc *make_binop(XokMalloc *xokMalloc, char *calcsign,
                      craastdesc *left , craastdesc *right);
craastdesc *make_assign(XokMalloc *xokMalloc, char *name, 
                         craastdesc *value , char *type);
craastdesc *make_call(XokMalloc *xokMalloc, char *name);
craastdesc *make_lvalue(XokMalloc *xokmalloc , char *name);
craastdesc *make_hasvar(XokMalloc *xokmalloc,
craastdesc *value, char *name, char *type);
craastdesc *make_cmp(XokMalloc *xokmalloc, char *cmpc,
                craastdesc *left , craastdesc *right);
craastdesc *make_condi(XokMalloc *xokmalloc, char *cmpc,
                craastdesc *left , craastdesc *right);
craastdesc *make_cbdesc(XokMalloc *xokmalloc, craastdesc *condi,
                               Tblock *block, craasttype type);
/* desclist func */
desclist *craP_desclist_create(XokMalloc *xm, int init_size);
void craP_desclist_free(XokMalloc *xm, desclist *dl);
int craP_desclist_push(XokMalloc *xm, desclist *dl, craastdesc *tree);
int craP_desclist_expand(XokMalloc *xm, desclist *dl);
/* Tblock func*/
Tblock *newTblock(XokMalloc *xm);
/* Istack func */
Istack *newIstack(XokMalloc *xm);
void pushI(Istack *stack, int i);
int popI(Istack *stack);
#define clearI(stack)\
 (stack)->p = 0; (stack)->count = 0;
#endif