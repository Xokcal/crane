#include <stdio.h>
#include <stdlib.h>
#include "xokmalloc.h"

#define MEMPOOL_SIZE 100 * 1024 * 1024 * 3

XokMalloc *
create_XokMalloc()
{
    XokMalloc *xokmalloc = (XokMalloc*)malloc(sizeof(XokMalloc));
    xokmalloc->xok_pool = (void*)malloc((MEMPOOL_SIZE) * sizeof(1));
    xokmalloc->offset = sizeof(0);
    xokmalloc->count = 0;
    xokmalloc->base_address = xokmalloc->xok_pool;
    return xokmalloc;
}

void* 
xalloc_XokMalloc(XokMalloc *xokmalloc , size_t size)
{
    xokmalloc->offset = xokmalloc->offset + size;
    xokmalloc->count++;
    void*p =  xokmalloc->xok_pool;
    xokmalloc->xok_pool = xokmalloc->xok_pool + size;
    return p;
}

void 
free_XokMalloc(XokMalloc *xokmalloc)
{
    free(xokmalloc->base_address);
    free(xokmalloc);
}

