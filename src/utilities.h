/*
 * header file utilities.h
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <stdio.h>
#include <stddef.h>

FILE *open_file(const char *file_name, const char *mode);

void *alloc_memory(size_t size);

int search_string(const char *str, const char *pattern);

int is_end_with_dot(char *path);
int dir_or_file(char *path);
#endif

