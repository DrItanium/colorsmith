#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <piGlow.h>
#include <string.h>
#include <errno.h>
#include "libcommon.h"
#include "piglowtranslationtable.h"


int main() {
   int i;
   size_t result;
   ColorsmithMicroOperation uop;
   byte value;
   byte *ptr;
   piGlowSetup(0);
   do {
      result = fread(&uop, sizeof(ColorsmithMicroOperation), 1, stdin);
      if (result == 1) {
         ptr = (byte*)uop;
         for(i = 0; i < LEDCount; i++, ptr++) {
            piGlow1(legarray[i], ringarray[i], *ptr);
         }
         delay(*ptr);
      } 
   } while(!feof(stdin) && !ferror(stdin));
   return 0;
}

