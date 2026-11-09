#include <stdio.h>
#include <string.h>
#include "cracode.h"
#include "../tool/xokmalloc/xokmalloc.h"
#include "craparse.h"
#include "cralex.h"
#include "cravm.h"
#include "craobject.h"
#include "types.h"

#define TYPES_DEFAULT_SIZE 300
#define TYPES_GROW_FACTOR  2

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
 if(op==11)strcomcat(s,"REASSIGN");
 if(op==12)strcomcat(s,"JMP_FALSE");
 if(op==13)strcomcat(s,"JMP_TRUE");
 if(op==14)strcomcat(s,"JMP");
 if(op==15)strcomcat(s,"CMP");
 if(op==16)strcomcat(s,"CONDI");
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
  op_calcblank(17, 4, blank);
  strcomcat(s, blank->str);
  strcomcat(s, p);
  strfree(blank);
  return 0;
 }
 if(preop==1){
  op_calcblank(17, 3, blank);
  strcomcat(s, blank->str);
  strcomcat(s, "+");
  strfree(blank);
  return 1;
}
 if(preop==2){
  op_calcblank(17, 3, blank);
  strcomcat(s, blank->str);
  strcomcat(s, "-");
  strfree(blank);
  return 1;
}
 if(preop==3){
  op_calcblank(17, 3, blank);
  strcomcat(s, blank->str);
  strcomcat(s, "/");
  strfree(blank);
  return 1;
}
 if(preop==4){
  op_calcblank(17, 3, blank);
  strcomcat(s, blank->str);
  strcomcat(s, "*");
  strfree(blank);
  return 1;
}
 if(preop==5){return 0;}
 if(preop==6){
  op_calcblank(17, 5, blank);
  strcomcat(s, blank->str);
  strcomcat(s,(vns->vn[op]));
  strclear(blank);
  strfree(blank);
  return 0;
 }
 if(preop==7){
  op_calcblank(17, 4, blank);
  strcomcat(s, blank->str);
  strcomcat(s, (tcalls->calls[op]));
  strfree(blank);
  return 0;
}
 if(preop==8){
  op_calcblank(17, 4, blank);
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
  op_calcblank(17, 4, blank);
  strcomcat(s, blank->str);
  strcomcat(s, types->types[op]);
  strfree(blank);
  return 0;
 }
 if(preop==10){
  op_calcblank(17, 3, blank);
  strcomcat(s, blank->str);
  strcomcat(s, vns->vn[op]);
  strfree(blank);
  return 0;
 }
 if(preop==11){
  op_calcblank(17, 8, blank);
  strcomcat(s, blank->str);
  strcomcat(s, vns->vn[op]);
  strfree(blank);
  return 0;
 }
 if(preop==12){
  op_calcblank(17, 9, blank);
  strcomcat(s, blank->str);
  char buf[32];
  snprintf(buf,sizeof(buf), "%d", op);
  char *p = buf;
  strcomcat(s, p);
  strfree(blank);
  return 0;
 }
 if(preop==13){
  op_calcblank(17, 8, blank);
  strcomcat(s, blank->str);
  char buf[32];
  snprintf(buf,sizeof(buf), "%d", op);
  char *p = buf;
  strcomcat(s, p);
  strfree(blank);
  return 0;
 }
 if(preop==14){
  op_calcblank(17, 3, blank);
  strcomcat(s, blank->str);
  char buf[32];
  snprintf(buf,sizeof(buf), "%d", op);
  char *p = buf;
  strcomcat(s, p);
  strfree(blank);
  return 0;
 }
 if(preop==15){
  op_calcblank(17, 3, blank);
  strcomcat(s, blank->str);
  strcomcat(s,(vns->vn[op]));
  strclear(blank);
  strfree(blank);
  return 0;
 }
 if(preop==16){
  op_calcblank(17, 5, blank);
  strcomcat(s, blank->str);
  strcomcat(s,(vns->vn[op]));
  strclear(blank);
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
     if (!targs_expand(xm, t)) return 0;}
 t->args[t->count] = param;
 t->count++;
 return 1;
}


static Ttypes *
types_create(XokMalloc *xm, int init_size)
{
 if (init_size <= 0) init_size = TYPES_DEFAULT_SIZE;
 Ttypes *t = (Ttypes *)
 xalloc_XokMalloc(xm, sizeof(Ttypes));
 if (!t) return NULL;
 t->types = (char **)
 xalloc_XokMalloc(xm, sizeof(char *) * init_size);
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
 char **new_types = (char **)
 xalloc_XokMalloc(xm, sizeof(char *) * new_size);
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
  bytecode *bc = (bytecode *)
  xalloc_XokMalloc(xm, sizeof(bytecode));
  if (!bc) return NULL;
  bc->codes = (int *)
  xalloc_XokMalloc(xm, sizeof(int) * init_size);
  bc->values = (value *)
  xalloc_XokMalloc(xm, sizeof(value) * init_size);
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
       free(bc->values[i].v.s);
      }
  }
  if (bc->codes){}
}


int
bc_expand(XokMalloc *xm, bytecode *bc)
{
  if (!bc) return 0;
  int new_size = bc->size * 2;
  int *new_codes = (int *)
  xalloc_XokMalloc(xm, sizeof(int) * new_size);
  if (!new_codes) return 0;
  memcpy(new_codes, bc->codes, sizeof(int) * bc->count);
  bc->codes = new_codes;
  bc->size  = new_size;
  value *new_values = (value *)
  xalloc_XokMalloc(xm, sizeof(value) * new_size);
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
  varnamestk *vs = (varnamestk *)
  xalloc_XokMalloc(xm, sizeof(varnamestk));
  if (!vs) return NULL;
  vs->vn = (char **)
  xalloc_XokMalloc(xm, sizeof(char *) * init_size);
  vs->slots = (int *)
  xalloc_XokMalloc(xm, sizeof(int) * init_size);
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
  char **new_vn = (char **)
  xalloc_XokMalloc(xm, sizeof(char *) * new_size);
  int *new_slots = (int *)
  xalloc_XokMalloc(xm, sizeof(int) * new_size);
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
   if(!vnstk_expand(xm, vs)) return 0;}
  vs->vn[vs->count]    = strdup(name);
  vs->slots[vs->count] = slot;
  vs->count++;
  return 1;
}

static int
vnstk_find(varnamestk *vs, const char *name)
{
  for (int i = 0; i < vs->count; i++) {
   if (strcmp(vs->vn[i], name) == 0)return vs->slots[i];}
  return -1;
}


/*
** package bytecode init struct and essential number.
*/
codesState packagecodestate(XokMalloc *xm){
 codesState cs;
 cs.xm = xm;
 cs.bc = bc_create(xm , BC_DEFAULT_SIZE);
 cs.types = types_create(xm , TYPES_DEFAULT_SIZE);
 cs.vns = vnstk_create(xm , 300000);
 cs.targs = targs_create(xm, TARGS_DEFAULT_SIZE);
 cs.tcalls = tcalls_create(xm, TCALLS_DEFAULT_SIZE);
 cs.dl_c = (int*)xalloc_XokMalloc(xm, sizeof(int));
 cs.t_idx = (int*)xalloc_XokMalloc(xm, sizeof(int));
 cs.vnsi = (int*)xalloc_XokMalloc(xm, sizeof(int));
 cs.tcallsi = (int*)xalloc_XokMalloc(xm, sizeof(int));
 (*cs.dl_c) = 0;
 (*cs.t_idx) = 0;
 (*cs.vnsi) = 0;
 (*cs.tcallsi) = 0;
 return cs;
}


/*
** parse assign calculate sign to byte code. 
*/
static int
signemit(XokMalloc *xm , char *s , bytecode *bc)
{
  if(strcmp(s , "+")==0)return bc_emit(xm , bc , OP_ADD);
  if(strcmp(s , "-")==0)return bc_emit(xm , bc , OP_SUB);
  if(strcmp(s , "*")==0)return bc_emit(xm , bc , OP_MUL);
  if(strcmp(s , "/")==0)return bc_emit(xm , bc , OP_DIV);
}


static void treenumber(codesState cs, craastdesc *node, char *type){
 value v;
 v_type_insert(type , &v , &node->as.num);
 bc_emit(cs.xm, cs.bc, OP_PUSH);
 bc_emit(cs.xm , cs.bc , cs.bc->v_count);
 /* push struct value stack */
 bc_add_constant(cs.xm, cs.bc , v);
}


static void treelvalue(codesState cs, craastdesc *node){
 bc_emit(cs.xm, cs.bc, OP_PUSH_VAR);
 vnstk_push(cs.xm, cs.vns, node->as.lvname, (*cs.vnsi));
 bc_emit(cs.xm, cs.bc, (*cs.vnsi)++);
}


/* 
** left-right-mid recursion，
** leftest is highest priority binop, priority tackle leftest binop
** left-num and right-num, rightest is low priority, can tabckle later 
*/
static void treebinop(codesState cs, craastdesc *node, char *type){
 assignencode(cs, node->as.binop.left, type);
 assignencode(cs, node->as.binop.right, type);
 signemit(cs.xm, node->as.binop.calcsign, cs.bc);
}


/*
** assign tree parse to bytecode main execute. 
*/
void assignencode(codesState cs, craastdesc *node, char *type){
 switch(node->kind){
  case AST_NUM:treenumber(cs, node, type);break;
  case AST_LVNAME:treelvalue(cs, node);break;
  case AST_BINOP:treebinop(cs, node, type);break;
  default:break;}
 return;
}


static void astassignexe(codesState cs, craastdesc *curdesc, int *dl){
 craastdesc *value = curdesc->as.assign.value; /* get desc assign 
 value tree, just go parse tree to codes */
 char *type = curdesc->as.assign.type; /* data type string information */
 if(value==NULL)
  goto undef;
 assignencode(cs, value, type);
 undef:
 bc_emit(cs.xm , cs.bc , OP_STORE);
 vnstk_push(cs.xm , cs.vns , curdesc->as.assign.name , (*cs.vnsi));
 bc_emit(cs.xm , cs.bc , (*cs.vnsi)++); /* variable name index */
 // char *type = curdesc->as.assign.type;
 types_push(cs.xm , cs.types , type); /* variable type store Ttype arrays */
 /* need store variable type, like:{[STORE : a]->[int]} 
 when STORE variable name opinstruct is end !!! */
 bc_emit(cs.xm, cs.bc, OP_TYPE);
 bc_emit(cs.xm , cs.bc , (*cs.t_idx)++);
 //#define v1v2
 #ifdef v1v2
 logi("bc.values.v[1]" , bc->values[0].v.i);
 logi("bc.values.v[2]" , bc->values[1].v.i);
 #endif
 (*dl)++;
}


/*
** by desc.call.type judge specific unkey type, first push 
** codes op_func or op_undefvar or .. desc.call.Tparams push
** Targs, and push codes store Targs has pushed index.
*/
static void astcallexe(codesState cs, craastdesc *desc, int *dl){
 int argsi = 0;
 while(argsi < desc->as.call.argsi){
  bc_emit(cs.xm, cs.bc, OP_LOAD);
  bc_emit(cs.xm, cs.bc, cs.targs->count);
  targs_push(cs.xm, cs.targs, getcallargs(desc, argsi));
  argsi++;
 }
 bc_emit(cs.xm, cs.bc, OP_CALL);
 bc_emit(cs.xm, cs.bc, (*cs.tcallsi)++);
 tcalls_push(cs.xm, cs.tcalls, getcallfname(desc));
 /* current codes like: { op_load:[p1],
  op_load:[p2], op_call:[call] } */
 //#define targsP
 #ifdef targsP
 for(int i=0;i<targs->count;i++)
  logc("vm_Targs",targs->args[i].originstr);
 #endif
 (*dl)++;
}


static void asthasvarexe(codesState cs, craastdesc *desc, int *dl){
 char *type = desc->as.hasvar.type;
 assignencode(cs, desc->as.hasvar.value, type);
 bc_emit(cs.xm, cs.bc, OP_STORE_HASVAR);
 vnstk_push(cs.xm , cs.vns , desc->as.hasvar.name , (*cs.vnsi));
 bc_emit(cs.xm , cs.bc , (*cs.vnsi)++);  /* variable name index */
 (*dl)++;
 return;
}


static void nodecondiexe(codesState cs, craastdesc *node,
                    Istack *andtstack, Istack *orstack){
 if(strcmp(node->as.binop.calcsign, "and")==0){
  /* Use temp stack record continous 'and' logic block, 
  and all assign by next or's first instruction code value 
  when reach 'or'. Then clear and temp stack*/
  bc_emit(cs.xm, cs.bc, OP_JMP_FALSE);
  pushI(andtstack, cs.bc->count);
  bc_emit(cs.xm, cs.bc, 0);
 }else{
  /* Reach 'or' first push current 'or' to orstack.
  Then next instruction code assignto and temp stack. */
  bc_emit(cs.xm, cs.bc, OP_JMP_TRUE);
  pushI(orstack, cs.bc->count);
  bc_emit(cs.xm, cs.bc, 0);
  if(node->as.binop.right != NULL){
   while(andtstack->count > 0){
    /* Revise codes[] contain and temp stack has record
    index to bc current index. */
    cs.bc->codes[popI(andtstack)] = cs.bc->count;
   }
  }
 }
}

/*
** Only give ifdesc node's condi desc tree.
*/
static void ifcondiexe(codesState cs, craastdesc *node,
                   Istack *andtstack, Istack *orstack){
 if(node->kind == AST_CMP){
  assignencode(cs, node->as.binop.left, "int");
  assignencode(cs, node->as.binop.right, "int");
  bc_emit(cs.xm, cs.bc, OP_CMP);
  vnstk_push(cs.xm , cs.vns , node->as.binop.calcsign , (*cs.vnsi));
  bc_emit(cs.xm , cs.bc , (*cs.vnsi)++);
 }
 if(node->kind == AST_CONDI){
  ifcondiexe(cs, node->as.binop.left, andtstack, orstack);
  /* record condition node like 'and' 'or', and
   judge use 'OP_JMP_FALSE' or 'OP_JMP_TRUE' */
  bc_emit(cs.xm, cs.bc, OP_CONDI);
  vnstk_push(cs.xm , cs.vns , node->as.binop.calcsign , (*cs.vnsi));
  bc_emit(cs.xm , cs.bc , (*cs.vnsi)++);
  nodecondiexe(cs, node, andtstack, orstack);
  ifcondiexe(
  cs, node->as.binop.right, andtstack, orstack);
 }
}


/*
** AST_IF code parse main executor, because if outer parse main function
** and if block main parse function don's shared int *dlindex, 
** so need alone transfor tree list count.
*/
static void ifdescexe(codesState *cs, craastdesc *curdesc, int *dlindex){
 Istack *andtstack = newIstack(cs->xm);
 Istack *orstack = newIstack(cs->xm);
 ifcondiexe(*cs, curdesc->as.ifdesc.condi, andtstack, orstack);
 /* Push orstack judge end 'or' logic again */
 bc_emit(cs->xm, cs->bc, OP_JMP_TRUE);
 pushI(orstack, cs->bc->count);
 bc_emit(cs->xm, cs->bc, 0);
 /* If all condition not match, JMP to if out */
 bc_emit(cs->xm, cs->bc, OP_JMP);
 /* temp record current 'OP_JMP' instructction in codes index */
 int ifjmpoutindex = cs->bc->count;
 bc_emit(cs->xm, cs->bc, 0);
 /* All 'or' stack revise to current codes index,
  because current bc index is entry if block first instruction index */
 while(orstack->count > 0)
  cs->bc->codes[popI(orstack)] = cs->bc->count;
 /* Use craC_bytecode_parse() execute ifdesc block tree. */
 int blockdl = 0;
 craC_bytecode_parse(
 cs->xm, curdesc->as.ifdesc.block->treelist, 
 cs, "block", &blockdl);
 cs->bc->codes[ifjmpoutindex] = cs->bc->count;
 (*dlindex)++;
}


/*
** bytecode parse main execute.
*/
vmState* craC_bytecode_parse(XokMalloc *xm ,
 desclist *dl, codesState *cs, char *type, int *dlindex){
 while((*dlindex) < dl->count){
  craastdesc *curdesc = dl->desclist[(*dlindex)];
  switch(curdesc->kind){
   case AST_HASVAR:{
    asthasvarexe(*cs, curdesc, dlindex);
    break;
   }
   case AST_ASSIGN:{
    astassignexe(*cs, curdesc, dlindex);
    break;
   }
   case AST_CALL:{
    astcallexe(*cs, curdesc, dlindex);
    break;
   }
   case AST_IF:{
    ifdescexe(cs, curdesc, dlindex);
    break;
   }
   default:break;
  }
 }
 if(strcmp(type, "main")==0)
  bc_emit(xm , cs->bc , OP_HALT);
 #define codesvisualization
 #ifdef codesvisualization
 craC_codes_log(cs->bc);
 op_visualization(cs->bc, cs->vns , cs->tcalls, cs->targs, cs->types);
 #endif
 if(strcmp(type, "main")==0){
  return vm_create(xm, cs->bc, cs->vns,
  cs->types, cs->targs, cs->tcalls, 2500000);
 }
 else{}
}

