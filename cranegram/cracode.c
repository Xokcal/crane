#include <stdio.h>
#include <string.h>
#include "cracode.h"
#include "../tool/xokmalloc/xokmalloc.h"
#include "craparse.h"
#include "cralex.h"
#include "cravm.h"

#define BYTECODE_GROW_FACTOR  2
#define craC_codes_log(bc) \
  for(int i = 0; i < (bc)->count; i++) log_i("BC" , (bc)->codes[i]);

/* desc.call */
#define getcallargs(desc, i)\
 (desc)->as.call.args[(i)]

#define getcallfname(desc)\
 (desc)->as.call.fname

#define BC_DEFAULT_SIZE 300

/* ************** String func tackle ***************** */

#define strclear(s) delete_all((s))

#define strfree(s) string_free((s))

#define strcomcat(s, s2) combine_tail((s), (s2))

/*
** by struct vartype convert to char* origin type
** like: {TYPE_INT ---> "int"},{TYPE_STRING ---> "string"}
*/
const char*
typetostr(vartype type)
{
 switch (type) {
  case TYPE_INT:return "int";
  case TYPE_STRING: return "string";
  case TYPE_FLOAT:return "float";
  case TYPE_DOUBLE:return "double";
  case TYPE_CHAR:return "char";
  case TYPE_LONG:return "long";
  default:return "unknown";
 }
}


/* 
** void *vv is value , vm by char *type judge 
** var type and push stack
*/
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


#include "cgm_file.h"

static void
op_comcatop(int op, String *s)
{
 if(op==0)strcomcat(s,"PUSH");
 if(op==1)strcomcat(s,"ADD");
 if(op==2)strcomcat(s,"SUB");
 if(op==3)strcomcat(s,"DIV");
 if(op==4)strcomcat(s,"MUL");
 if(op==5)strcomcat(s,"EXIT");
 if(op==6)strcomcat(s,"STORE");
 if(op==7)strcomcat(s,"CALL");
 if(op==8)strcomcat(s,"LOAD");
 if(op==9)strcomcat(s,"TYPE");
 if(op==10)strcomcat(s,"VAR");
}

static void
op_calcblank(int max, int self, String *s)
{for(int i=self;i<max;i++)strcomcat(s, " ");}

static int
op_codeparse(int op, int preop, int *opi, String *s, bytecode *bc,
    varnamestk *vns, Tcalls *tcalls, Targs *targs, Ttypes *types){
 String *blank = create_string("");
 if(preop==0){
  char buf[32];
  snprintf(buf,sizeof(buf), "%d", bc->values[op].v.i);
  char *p = buf;
  op_calcblank(7, 4, blank);
  strcomcat(s, blank->str);
  strcomcat(s, p);
  strfree(blank);
  return 0;
 }
 if(preop==1){
  op_calcblank(7, 3, blank);
  strcomcat(s, blank->str);
  strcomcat(s, "+");
  strfree(blank);
  return 1;
}
 if(preop==2){
  op_calcblank(7, 3, blank);
  strcomcat(s, blank->str);
  strcomcat(s, "-");
  strfree(blank);
  return 1;
}
 if(preop==3){
  op_calcblank(7, 3, blank);
  strcomcat(s, blank->str);
  strcomcat(s, "/");
  strfree(blank);
  return 1;
}
 if(preop==4){
  op_calcblank(7, 3, blank);
  strcomcat(s, blank->str);
  strcomcat(s, "*");
  strfree(blank);
  return 1;
}
 if(preop==5){return 0;}
 if(preop==6){
  op_calcblank(7, 5, blank);
  strcomcat(s, blank->str);
  strcomcat(s,(vns->vn[op]));
  strclear(blank);
  strfree(blank);
  return 0;
 }
 if(preop==7){
  op_calcblank(7, 4, blank);
  strcomcat(s, blank->str);
  strcomcat(s, (tcalls->calls[op]));
  strfree(blank);
  return 0;
}
 if(preop==8){
  op_calcblank(7, 4, blank);
  strcomcat(s, blank->str);
  if(targs->args[op].pty==PRM_STRING){
   strcomcat(s, "\"");
   strcomcat(s, targs->args[op].originstr);
   strcomcat(s, "\"");
  }else
   strcomcat(s, targs->args[op].originstr);
  strfree(blank);
  return 0;
}
 if(preop==9){
  op_calcblank(7, 4, blank);
  strcomcat(s, blank->str);
  strcomcat(s, types->types[op]);
  strfree(blank);
  return 0;
 }
 if(preop==10){
  op_calcblank(8, 4, blank);
  strcomcat(s, blank->str);
  strcomcat(s, vns->vn[op]);
  strfree(blank);
  return 0;
 }
}


static void
op_visualization(bytecode *bc, varnamestk *vns,
 Tcalls *tcalls, Targs *targs, Ttypes *types){
 String *s = create_string("");
 int opi = 0;
 int j = 0;
 while(opi<bc->count){
  int op = bc->codes[opi];
  int preop = bc->codes[(opi)-1];
  if(j==1){
   int conver = op_codeparse(op, preop, &opi,
   s, bc, vns, tcalls, targs, types);
   strcomcat(s, "\n");
   opi++;
   j = 0;
  }
  else{ // j = 0
   op_comcatop(op, s);
   if(op==1||op==2||op==3||op==4){
    int curop = bc->codes[opi];
    int conver = op_codeparse(op, op, &opi,
    s, bc, vns, tcalls, targs, types);
    strcomcat(s, "\n");
    j = 0;
    opi++;
   }else{
    opi++;
    j = 1;
   }
  }
 }
 write_file("test/vmcodes.lua", s->str);
}


#define TCALLS_DEFAULT_SIZE 300
#define TCALLS_GROW_FACTOR  2


Tcalls *
tcalls_create(XokMalloc *xm, int init_size)
{
 if (init_size <= 0) init_size = TCALLS_DEFAULT_SIZE;
 Tcalls *tc = (Tcalls *)
 xalloc_XokMalloc(xm, sizeof(Tcalls));
 if (!tc) return NULL;
 tc->calls = (char **)
 xalloc_XokMalloc(xm, sizeof(char *) * init_size);
 if (!tc->calls) {
  return NULL;
 }
 tc->size  = init_size;
 tc->count = 0;
 return tc;
}


void
tcalls_free(XokMalloc *xm, Tcalls *tc)
{
 if (!tc) return;
 if (tc->calls) {
  for (int i = 0; i < tc->count; i++) {
   if (tc->calls[i]) {
    free(tc->calls[i]); 
   }
  }
  free(tc->calls);
 }
 free(tc);
}


int
tcalls_expand(XokMalloc *xm, Tcalls *tc)
{
 if (!tc) return 0;
 int new_size = tc->size * TCALLS_GROW_FACTOR;
 char **new_calls = (char **)
 xalloc_XokMalloc(xm, sizeof(char *) * new_size);
 if (!new_calls) return 0;
 memcpy(new_calls, tc->calls, sizeof(char *) * tc->count);
 tc->calls = new_calls;
 tc->size  = new_size;
 return 1;
}


int
tcalls_push(XokMalloc *xm, Tcalls *tc, const char *call_name)
{
 if (!tc || !call_name)return 0;
 if (tc->count >= tc->size) {
  tcalls_expand(xm, tc);
 }
 tc->calls[tc->count] = strdup(call_name);
 tc->count++;
 return 1;
}

const char *
tcalls_pop(Tcalls *tc)
{
 if (!tc || tc->count <= 0) return NULL;
 const char *name = tc->calls[tc->count - 1];
 tc->count--;
 return name;
}


#define TARGS_DEFAULT_SIZE 300
#define TARGS_GROW_FACTOR  2


/*
** Targs is collect craparse.c's unkey[like:function->(print(a, b))]
** (a), (b) args message, and push Targs by bytecode(index) get, and 
** virtual machine tackle bytecode corresponding Tparam op 
** instruct by bytecode(index).
** struct Targs's create, contain Tparam array, size,..
*/
Targs *
targs_create(XokMalloc *xm, int init_size)
{
 if (init_size <= 0) init_size = TARGS_DEFAULT_SIZE;
 Targs *t = (Targs *)
 xalloc_XokMalloc(xm, sizeof(Targs));
 if (!t) return NULL;
 t->args = (Tparam *)
 xalloc_XokMalloc(xm, sizeof(Tparam) * init_size);
 if (!t->args) {
  return NULL;
 }
 t->size  = init_size;
 t->count = 0;
 return t;
}


void
targs_free(XokMalloc *xm, Targs *t)
{
 if (!t) return;
 if (t->args) {
  for (int i = 0; i < t->count; i++) {
   if (t->args[i].originstr) {
    free(t->args[i].originstr);
   }
  }
  free(t->args);
 }
 free(t);
}


int
targs_expand(XokMalloc *xm, Targs *t)
{
 if (!t) return 0;
 int new_size = t->size * TARGS_GROW_FACTOR;
 Tparam *new_args = (Tparam *)
 xalloc_XokMalloc(xm, sizeof(Tparam) * new_size);
 if (!new_args) return 0;
 memcpy(new_args, t->args, sizeof(Tparam) * t->count);
 t->args = new_args;
 t->size = new_size;
 return 1;
}


/*
** targs stack push struct Tparam
*/
int
targs_push(XokMalloc *xm, Targs *t, Tparam param)
{
 if (!t) return 0;
 if (t->count >= t->size) {
     if (!targs_expand(xm, t)) return 0;
 }
 t->args[t->count] = param;
 t->count++;
 return 1;
}


#define TYPES_DEFAULT_SIZE 300
#define TYPES_GROW_FACTOR  2


static Ttypes *
types_create(XokMalloc *xm, int init_size)
{
 if (init_size <= 0) init_size = TYPES_DEFAULT_SIZE;
 Ttypes *t = (Ttypes *)xalloc_XokMalloc(xm, sizeof(Ttypes));
 if (!t) return NULL;
 t->types = (char **)xalloc_XokMalloc(xm, sizeof(char *) * init_size);
 if (!t->types)
  return NULL;
 t->size  = init_size;
 t->count = 0;
 return t;
}

static void
types_free(XokMalloc *xm, Ttypes *t)
{
 if (!t) return;
 if (t->types) {
  for (int i = 0; i < t->count; i++) {
   if (t->types[i]) free(t->types[i]);
  }
  free(t->types);
 free(t);
 }
}

/* 扩容 */
static int
types_expand(XokMalloc *xm, Ttypes *t)
{
 if (!t) return 0;
 int new_size = t->size * TYPES_GROW_FACTOR;
 char **new_types = (char **)xalloc_XokMalloc(xm, sizeof(char *) * new_size);
 if (!new_types) return 0;
 memcpy(new_types, t->types, sizeof(char *) * t->count);
 t->types = new_types;
 t->size  = new_size;
 return 1;
}


static int
types_push(XokMalloc *xm, Ttypes *t, const char *type_str)
{
 if (!t || !type_str) return 0;
 if (t->count >= t->size)
  if (!types_expand(xm, t)) return 0;
 t->types[t->count] = strdup(type_str);
 t->count++;
 return 1;
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
signemit(XokMalloc *xm , char *s , bytecode *bc)
{
  if(strcmp(s , "+")==0)return bc_emit(xm , bc , OP_ADD);
  if(strcmp(s , "-")==0)return bc_emit(xm , bc , OP_SUB);
  if(strcmp(s , "*")==0)return bc_emit(xm , bc , OP_MUL);
  if(strcmp(s , "/")==0)return bc_emit(xm , bc , OP_DIV);
}


static void
assignencode(XokMalloc *xm, craastdesc *node, varnamestk *vns,
                        bytecode *bc , char *type, int *vnsi){
 if (node->kind == AST_NUM) { /* single tackle node
  is AST_NUM, if is binop node don't push*/
  value v;
  v_type_insert(type , &v , &node->as.num);
  bc_emit(xm, bc, OP_PUSH);
  bc_emit(xm , bc , bc->v_count);
  /* push struct value stack */
  bc_add_constant(xm, bc , v);
  return;
 }
 if(node->kind == AST_LVNAME){
  bc_emit(xm, bc, OP_PUSH_VAR);
  vnstk_push(xm, vns, node->as.lvname, (*vnsi));
  bc_emit(xm, bc, (*vnsi)++);
  return;
 }
 if (node->kind == AST_BINOP) { /* left-right-mid recursion，
  leftest is highest priority binop, priority tackle leftest binop
  left-num and right-num, rightest is low priority, can tabckle later */
  assignencode(xm, node->as.binop.left, vns, bc , type, vnsi);
  assignencode(xm, node->as.binop.right, vns, bc , type, vnsi);
  signemit(xm, node->as.binop.calcsign, bc);
  return;
 }
 return;
}


static void
astassignexe(XokMalloc *xm, craastdesc *curdesc, Ttypes *types, 
bytecode *bc, varnamestk *vns, int *dl_c, int *t_idx, int *vnsi){
 craastdesc *value = curdesc->as.assign.value; /* get desc assign 
 value tree, just go parse tree to codes */
 char *type = curdesc->as.assign.type; /* data type string information */
 assignencode(xm , value , vns, bc , type, vnsi);
 bc_emit(xm , bc , OP_STORE);
 vnstk_push(xm , vns , curdesc->as.assign.name , *vnsi);
 bc_emit(xm , bc , (*vnsi)++); /* variable name index */
 // char *type = curdesc->as.assign.type;
 types_push(xm , types , type); /* variable type store Ttype arrays */
 /* need store variable type, like:{[STORE : a]->[int]} 
 when STORE variable name opinstruct is end !!! */
 bc_emit(xm, bc, OP_TYPE);
 bc_emit(xm , bc , (*t_idx)++);
 //#define v1v2
 #ifdef v1v2
 logi("bc.values.v[1]" , bc->values[0].v.i);
 logi("bc.values.v[2]" , bc->values[1].v.i);
 #endif
 (*dl_c)++;
}


/*
** by desc.call.type judge specific unkey type, first push 
** codes op_func or op_undefvar or .. desc.call.Tparams push
** Targs, and push codes store Targs has pushed index.
*/
static void
astcallexe(XokMalloc *xm, craastdesc *desc, Targs *targs,
  Tcalls *tcalls, bytecode *bc, int *dl_c, int *tcallsi){
 int argsi = 0;
 while(argsi < desc->as.call.argsi){
  bc_emit(xm, bc, OP_LOAD);
  bc_emit(xm, bc, targs->count);
  targs_push(xm, targs, getcallargs(desc, argsi));
  argsi++;
 }
 bc_emit(xm, bc, OP_CALL);
 bc_emit(xm, bc, (*tcallsi)++);
 tcalls_push(xm, tcalls, getcallfname(desc));
 /* current codes like: { op_load:[p1],
  op_load:[p2], op_call:[call] } */
 //#define targsP
 #ifdef targsP
 for(int i=0;i<targs->count;i++)
  logc("vm_Targs",targs->args[i].originstr);
 #endif
 (*dl_c)++;
}

/*
** bytecode parse main execute.
*/
vmState*
craC_bytecode_parse(XokMalloc *xm ,  desclist *dl)
{
 int dl_c = 0, /* desclist stack index */
 t_idx = 0,
 tcallsi = 0,
 vnsi = 0;
 Ttypes *types = types_create(xm , TYPES_DEFAULT_SIZE);
 bytecode *bc = bc_create(xm , BC_DEFAULT_SIZE);
 varnamestk *vns = vnstk_create(xm , 160000);
 Targs *targs = targs_create(xm, TARGS_DEFAULT_SIZE);
 Tcalls *tcalls = tcalls_create(xm, TCALLS_DEFAULT_SIZE);
 while(dl_c < dl->count){
  craastdesc *curdesc = dl->desclist[dl_c];
  switch(curdesc->kind){
   case AST_ASSIGN:{
    astassignexe(xm, curdesc, types,
    bc, vns, &dl_c, &t_idx, &vnsi);
    break;
   }
   case AST_CALL:{
    astcallexe(xm, curdesc, targs,
    tcalls, bc, &dl_c, &tcallsi);
    break;
   }
   default:break;
  }
 }
 bc_emit(xm , bc , OP_HALT);
 //#define codesvisualization
 #ifdef codesvisualization
 craC_codes_log(bc);
 op_visualization(bc, vns , tcalls, targs, types);
 #endif
 return vm_create(xm, bc, vns,
 types, targs, tcalls, 160);
}

