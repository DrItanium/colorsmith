#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "libcommon.h"

void uop_initialize(ColorsmithMicroOperation uop) {
	/* initialize the registers to zero */
	initialize(uop, sizeof(ColorsmithMicroOperation));
}


void initialize(byte* container, int count) {
	memset(container, 0, count);
}
