#include <stdio.h>
#include <stdlib.h>
#include "libcommon.h"

void error(const char* message, int code) {
   fprintf(stderr, "%s\n", message);
   exit(code);
}
