#include <stdio.h>
#include <errno.h>
#include "libcommon.h"

void error(const char* message, int code) {
   fprintf(stderr, "%s\n", message);
   exit(code);
}
