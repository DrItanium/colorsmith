#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "libcommon.h"

void custom_error(int code, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	exit(code);	
}
