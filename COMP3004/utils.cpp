#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

char* fileToBuffer(char* file) {
	FILE *fptr;
	long length;
	char *buf;
	fptr = fopen(file, "rb");
	if (!fptr) {
		fprintf(stderr, "failed to open %s\n", file);
		return NULL;
	}
	fseek(fptr, 0, SEEK_END);
	length = ftell(fptr);
	buf = (char *)malloc(length + 1);
	fseek(fptr, 0, SEEK_SET);
	fread(buf, length, 1, fptr);
	fclose(fptr);
	buf[length] = 0;
	return buf;
}
