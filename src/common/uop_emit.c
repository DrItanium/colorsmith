#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "libcommon.h"

void emit_uop(ColorsmithMicroOperation uop, FILE* out) {
   size_t result;
   result = fwrite(uop, sizeof(ColorsmithMicroOperation), 1, out);
   if (result != 1) {
      error("Couldn't output result to file", ferror(out));
   }
}

