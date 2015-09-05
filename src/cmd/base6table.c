/* Used to figure out that we are dealing with base 6 mathematics to interface
 * with the wiringPi libs with respect to the piGlow
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "libcommon.h"
#include "piglowtranslationtable.h"

int main() {
   int i;
   printf("Legend\n");
   printf("--------------\n");
   printf("- x: legarray\n");
   printf("- y: ringarray\n");
   printf("- z: led index\n");
   printf("--------------\n");
   printf("Format is: z <=> (x,y)\n");
   printf("\n\n");
   for (i = 0; i < 18; i++) {
      printf("%d <=> (%d, %d)\n", i, legarray[i], ringarray[i]);
   }
   return 0;
}
