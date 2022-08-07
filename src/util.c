#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "util.h"

int vasprintf(char **str, const char *fmt, va_list args) {
	int size = 0;
	va_list tmpa;
	va_copy(tmpa, args);
	size = vsnprintf(NULL, 0, fmt, tmpa);
	va_end(tmpa);
	if (size < 0) { return -1; }
	*str = (char *) malloc(size + 1);
	if (NULL == *str) { return -1; }
	size = vsprintf(*str, fmt, args);
	return size;
}

int asprintf(char **str, const char *fmt, ...) {
	int size = 0;
	va_list args;
	va_start(args, fmt);
	size = vasprintf(str, fmt, args);
	va_end(args);

	return size;
}
