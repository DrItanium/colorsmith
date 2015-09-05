#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "libcommon.h"

void uop_setring(ColorsmithMicroOperation uop, int ring, int intensity) {
	if (ring >= 0 && ring < 6) {
		uop[ring] = intensity;
		uop[ring+6] = intensity;
		uop[ring+12] = intensity;
	} else {
		error("panic: unknown ring combo provided", 3);
	}
}
