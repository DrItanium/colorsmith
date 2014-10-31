#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <piGlow.h>
#include <string.h>
#include <errno.h>
#include "types.h"


typedef byte ColorsmithMicroOperation[19];

int main() {
   int i, j;
   size_t result;
   ColorsmithMicroOperation uop;
   byte value;
   byte *ptr;
   piGlowSetup(0);
   do {
      result = fread(&uop, sizeof(ColorsmithMicroOperation), 1, stdin);
      if (result == 1) {
         ptr = (byte*)uop;
         for(i = 0; i < 3; i++) {
            for(j = 0; j < 6; j++) {
               piGlow1(i, j, *ptr);
               ptr++;
            }
         }
         delay(*ptr);
      } 
   } while(!feof(stdin) && !ferror(stdin));
   return 0;
}

