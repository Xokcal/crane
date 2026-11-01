//
// Created by 26432 on 2026/8/26.
//

#ifndef CRANEC_CGM_PARSE_H
#define CRANEC_CGM_PARSE_H

#include <stdlib.h>
#include <stdio.h>
#include "../tool/map/hashmap_int.h"
#include "../tool/String/String.h"
#include "../tool/xokmalloc/xokmalloc.h"

#define cratext(s) printf("%s" , (s));

#define CRA_DEBUG

#ifdef CRA_DEBUG
    #define LOG(content) printf("[DEBUG] %s\n" , (content))
    #define LOG_C(front , content) printf("[DEBUG] [%s] %s\n" , (front) , (content))
    #define LOG_CHR(front , content) printf("[DEBUG] [%s] %c\n" , (front) , (content))
    #define LOG_I(front , content) printf("[DEBUG] [%s] %d\n" , (front) , (content))
    #define cra_error(point) printf("%s cra error XCE note position: %s" , __TIME__ , (point));
    #define log(content) printf("[DEBUG] %s\n" , (content))
    #define log_c(front , content) printf("[DEBUG] [%s] %s\n" , (front) , (content))
    #define log_cr(front , content) printf("[DEBUG] [%s] %c\n" , (front) , (content))
    #define log_i(front , content) printf("[DEBUG] [%s] %d\n" , (front) , (content))
    #define logc(front , content) printf("[DEBUG] [%s] %s\n" , (front) , (content))
    #define logcr(front , content) printf("[DEBUG] [%s] %c\n" , (front) , (content))
    #define logi(front , content) printf("[DEBUG] [%s] %d\n" , (front) , (content))
#endif
//#define LOG(content)
//#define LOG_C(front , content)
//#define LOG_CHR(front , content)
//#define LOG_I(front , content)
//#define cra_error(point)
//#define log(content)
//#define log_c(front , content)
//#define log_cr(front , content)
//#define log_i(front , content)
//#define logc(front , content)
//#define logcr(front , content)
//#define logi(front , content)

typedef struct Hashmap Hashmap;

/* token parse 31 single char key. */
static const char signkeys[] = {'-','+','*','/',',',' ','.','>','=','<','{','}','(',')','&','^','|','\\','/','%','$','@','#','!','~',';','\"',':','\'','\n','\r'};

/* crane grammar 19 key. */
static const char *cranekeys[] = {"int","float","double","string","fn","print","if","for","in","static","long","char","struct","and","or","dand","dor","while","combine"};

typedef enum key_enum{
    PRINT_K,
    INT_K,
    PRINTF_K,
    DEFAULT
}KE;

typedef struct lextoken{
 String *token;
 long line;
}lextoken;

typedef struct lexState{
    lextoken **lextokens;
    int count;
}lexState;

typedef struct HAJIGA_ATM{
    int MONEY_FOR;
    String *ATM_AUTOAVATAR;
    String *NUTRIN_PAYER;
    String *ATM_SCREEN_0X1868632;
}HAJIGA_ATM;


int issinglekeys(const char c);
int iscranekeys(const char *s);
lexState *craL_lexexecute(XokMalloc *txm, String *origin_file_string);
#endif //CRANEC_CGM_PARSE_H
