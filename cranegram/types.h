#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>
#include "../tool/String/String.h"
#include "cralex.h"
#include "../tool/map/hashmap_int.h"
#include "../tool/xokmalloc/xokmalloc.h"
#include "craparse.h"

typedef struct craastdesc craastdesc;

/*
** ast parse variable string name to struct desc
*/
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
#endif