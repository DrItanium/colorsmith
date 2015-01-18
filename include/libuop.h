#ifndef _LIB_UOP_H
#define _LIB_UOP_H
/* Instruction particle */
enum {
	RegisterCount = FieldCount,
	DelayRegister = RegisterCount - 1,
};
typedef byte ColorsmithMicroOperation[19];

void uop_emit(ColorsmithMicroOperation uop, FILE* output);
void uop_setleg(ColorsmithMicroOperation uop, int leg, int intensity);
void uop_setled(ColorsmithMicroOperation uop, int leg, int ring, int intensity);
void uop_initialize(ColorsmithMicroOperation uop);

#endif 
