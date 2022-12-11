#ifndef PRINT_FUNCTION_H
#define PRINT_FUNCTION_H

#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
 #include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_list_entier(char * list, size_t size,char* name);

void print_list_chaine(char * list, size_t size,char* name);

void print_chaine(char* mystring, size_t size_string,char* name);

void print_struct_header(char *buffer);


#endif