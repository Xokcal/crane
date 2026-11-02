#ifndef craobject_h
#define craobject_h

#include <stdio.h>

/*
** use to various need to judged variable type condition
*/
typedef enum vartype{
 TYPE_INT,      /* [0]type int */
 TYPE_STRING,   /* [1]type string */
 TYPE_FLOAT,    /* [2]type float */
 TYPE_DOUBLE,   /* [3]type double */
 TYPE_CHAR,     /* [4]type char */
 TYPE_LONG,     /* [5]type long */
 TYPE_DLONG,    /* [6]type longlong */
 TYPE_UNKOWN    /* [7]not kown type */
}vartype;

/* store crane variable(int , float , long ,string,...)
type value. struct type store variable enum vartype */
typedef struct value{
  /* vartype enum type, use to match*/
 vartype type;
 /* variable name like: int a = 10;
                            | 
                            `---------> name*/
 char *vname; /* variable name string value */
 char *vtype; /* authentic variable type,
 like: {int, double, long, float, string} */
 union
 {
   int        i;     /* authentic variable type 'int' */
   float      f;     /* authentic variable type 'float' */
   char       *s;    /* authentic variable type 'string' */
   char       c;     /* authentic variable type 'char' */
   double     d;     /* authentic variable type 'double' */
   long       l;     /* authentic variable type 'long' */
   long long  ll;    /* authentic variable type 'longlong' */
 }v;
}value;

#endif