#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "libcommon.h"

void uop_initialize(ColorsmithMicroOperation uop) {
   /* initialize the registers to zero */
	memset(uop, 0, sizeof(ColorsmithMicroOperation));
}

