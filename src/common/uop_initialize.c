#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "libcommon.h"

void uop_initialize(ColorsmithMicroOperation uop) {
   /* initialize the registers to zero */
   int i;
   for(i = 0; i < RegisterCount; i++) {
      uop[i] = 0;
   }
}

