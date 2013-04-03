#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>

#include "utilities.h"

FILE *open_file(const char *file_name, const char *mode)
{
	FILE *fp;

	if ((fp = fopen(file_name, mode)) == NULL) {
		fprintf(stderr, "file %s open error\n", file_name);
		exit(-1);
	}
	return fp;
}

void *alloc_memory(size_t size)
{
	void *mem;

	mem = malloc(size);
	if (mem == NULL) {
		fprintf(stderr, "memory allocation error\n");
		exit(-1);
	}
	return mem;
}

int search_string(const char *str, const char *pattern)
{
	const char *p, *q;
	assert(str != NULL);
	assert(pattern != NULL);

	p = str;
	while (*p != '\0') {
		const char *r = p;

		q = pattern;
		while (*r != '\0' && *q != '\0' && *r++ == *q++)
			;
		if (*q == '\0')
			return p - str;
		p++;
	}
	return -1;
}

