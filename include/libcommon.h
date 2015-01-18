#ifndef _COMMON_H
#define _COMMON_H
typedef unsigned char byte;
typedef unsigned short ushort;
/* Instruction particle */
enum {
	LEDCount = 18,
	FieldCount = LEDCount + 1,
	RegisterCount = FieldCount,
	DelayRegister = RegisterCount - 1,
};
typedef byte ColorsmithMicroOperation[19];

void uop_emit(ColorsmithMicroOperation uop, FILE* output);
void uop_setleg(ColorsmithMicroOperation uop, int leg, int intensity);
void uop_setled(ColorsmithMicroOperation uop, int leg, int ring, int intensity);
void uop_setring(ColorsmithMicroOperation uop, int ring, int intensity);
void uop_initialize(ColorsmithMicroOperation uop);


void error(const char* message, int code);
#endif 
