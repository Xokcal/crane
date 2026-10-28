//
// Created by 26432 on 2026/8/26.
//

#include <stdio.h>
#include <stdlib.h>
#include "../tool/String/String.h"
#include "../tool/log/log.h"

String *open_file(char *URL) {
 FILE *file = fopen(URL, "rb");
 if (!file) {
  LOG_ERROR("open cr file fail.", "OPEN_FILE");
  return NULL;
 }
 fseek(file, 0, SEEK_END);
 long file_size = ftell(file);
 fseek(file, 0, SEEK_SET);
 char *buffer = (char *)malloc(file_size + 1);
 fread(buffer, 1, file_size, file);
 buffer[file_size] = '\0';
 fclose(file);
 String *tokens = create_string(buffer); 
 free(buffer);
 return tokens;
}

void write_file(const char *URL, char *contet){
 FILE *file = fopen(URL,"w");
 if(file == NULL){printf("write_file open file error");}
 fprintf(file, contet);
 fclose(file);
}