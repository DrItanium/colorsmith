#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "types.h"
#include "libcommon.h"
#include "piglowconstants.h"
#include "libuop.h"

void emit_uop(ColorsmithMicroOperation uop, FILE* out) {
   size_t result;
   result = fwrite(uop, sizeof(ColorsmithMicroOperation), 1, out);
   if (result != 1) {
      error("Couldn't output result to file", ferror(out));
   }
}

void uop_setleg(ColorsmithMicroOperation uop, int leg, int intensity) {
   int i, begin, end;
   switch (leg) {
      case 0:
         begin = 0;
         end = 6;
         break;
      case 1:
         begin = 6;
         end = 12;
         break;
      case 2:
         begin = 12;
         end = 18;
         break;
      default:
         error("panic: unknown leg provided to be set!", 4);
   }
   for(i = begin; i < end; i++) {
      uop[i] = intensity;
   }
}
void uop_setled(ColorsmithMicroOperation uop, int leg, int ring, int intensity) {
   // we end up with base 6 math since each leg has 6 entries and each ring has
   // three so we can come up with a base six math layout
   int index;
   index = (6 * leg) + ring;
   if (index >= 0 && index < 18) {
		uop[index] = intensity;
   } else {
      //print this and use lisp syntax because it is far more readable
      fprintf(stderr, "got illegal index %d from (+ (* 6 %d) %d)\n", index, leg, ring);
      error("panic: see above line!", 5);
   }
}

void uop_initialize(ColorsmithMicroOperation uop) {
   /* initialize the registers to zero */
   int i;
   for(i = 0; i < RegisterCount; i++) {
      uop[i] = 0;
   }
}

