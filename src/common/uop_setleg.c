#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "libcommon.h"

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
