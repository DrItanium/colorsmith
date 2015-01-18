#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "libcommon.h"

void uop_setled(ColorsmithMicroOperation uop, int leg, int ring, int intensity) {
   // we end up with base 6 math since each leg has 6 entries and each ring has
   // three so we can come up with a base six math layout
   int index;
   index = (6 * leg) + ring;
   if (index >= 0 && index < 18) {
		uop[index] = intensity;
   } else {
      //print this and use lisp syntax because it is far more readable
      custom_error(5, "panic: got illegal index %d from (+ (* 6 %d) %d)\n", index, leg, ring);
   }
}

