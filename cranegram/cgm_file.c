//
// Created by 26432 on 2026/8/26.
//

#include <stdio.h>
#include <stdlib.h>
#include "../tool/String/String.h"
#include "../tool/log/log.h"

String *open_file(char* URL){
    FILE *file = fopen(URL , "r");
    if(file == NULL){
        LOG_ERROR("open cr file fail." , "OPEN_FILE");
        return NULL;
    }
    String *tokens = create_string("");
    char buf[2];
    while(fgets(buf , sizeof(buf) , file))
        combine_tail_char(tokens , buf[0]);
    return tokens;
}

void write_file(const char *URL, char *contet){
 FILE *file = fopen(URL,"w");
 if(file == NULL){printf("write_file open file error");}
 fprintf(file, contet);
 fclose(file);
}