#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "libcommon.h"

void uop_emit(ColorsmithMicroOperation uop, FILE* out) {
   size_t result = fwrite(uop, sizeof(ColorsmithMicroOperation), 1, out);
   if (result != 1) {
      error("Couldn't output result to file", ferror(out));
   }
}

