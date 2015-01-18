#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "libcommon.h"

void uop_setring(ColorsmithMicroOperation uop, int ring, int intensity) {
   switch(ring) {
      case 0:
         uop[0] = intensity;
         uop[6] = intensity;
         uop[12] = intensity;
         break;
      case 1:
         uop[1] = intensity;
         uop[7] = intensity;
         uop[13] = intensity;
         break;
      case 2:
         uop[2] = intensity;
         uop[8] = intensity;
         uop[14] = intensity;
         break;
      case 3:
         uop[3] = intensity;
         uop[9] = intensity;
         uop[15] = intensity;
         break;
      case 4:
         uop[4] = intensity;
         uop[10] = intensity;
         uop[16] = intensity;
         break;
      case 5:
         uop[5] = intensity;
         uop[11] = intensity;
         uop[17] = intensity;
         break;
      default:
         error("panic: unknown ring combo provided", 3);
   }
}
