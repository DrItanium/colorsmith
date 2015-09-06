#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "libcommon.h"

void uop_emit(ColorsmithMicroOperation uop, FILE* out) {
	emit(uop, sizeof(ColorsmithMicroOperation), out);
}

void emit(byte* container, int count, FILE* out) {
	size_t result = fwrite(container, count, 1, out);
	if (result != 1) {
		error("Couldn't output result to file", ferror(out));
	}
}

