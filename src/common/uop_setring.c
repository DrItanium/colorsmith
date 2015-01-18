#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "libcommon.h"

void uop_setring(ColorsmithMicroOperation uop, int ring, int intensity) {
	int a, b, c;
	a = 0;
	b = 0;
	c = 0;
   switch(ring) {
      case 0:
			a = 0;
			b = 6;
			c = 12;
         break;
      case 1:
			a = 1;
			b = 7;
			c = 13;
         break;
      case 2:
			a = 2;
			b = 8;
			c = 14;
         break;
      case 3:
			a = 3;
			b = 9;
			c = 15;
         break;
      case 4:
			a = 4;
			b = 10;
			c = 16;
         break;
      case 5:
			a = 5;
			b = 11;
			c = 17;
         break;
      default:
         error("panic: unknown ring combo provided", 3);
   }
	uop[a] = intensity;
	uop[b] = intensity;
	uop[c] = intensity;
}
