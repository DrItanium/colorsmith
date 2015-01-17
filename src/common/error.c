#include <stdio.h>
#include <errno.h>
#include "types.h"

void error(const char* message, int code) {
   fprintf(stderr, "%s\n", message);
   exit(code);
}
