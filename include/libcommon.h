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

	LegLEDCount = LEDCount / 3,
	LegFieldCount = LegLEDCount + 1,
	LegRegisterCount = LegFieldCount,
	LegDelayRegister = LegRegisterCount - 1,
};
typedef byte ColorsmithMicroOperation[FieldCount];

void uop_emit(ColorsmithMicroOperation uop, FILE* output);
void uop_setleg(ColorsmithMicroOperation uop, int leg, int intensity);
void uop_setled(ColorsmithMicroOperation uop, int leg, int ring, int intensity);
void uop_setring(ColorsmithMicroOperation uop, int ring, int intensity);
void uop_initialize(ColorsmithMicroOperation uop);

void emit(byte* container, int length, FILE* output);
void initialize(byte* container, int length);

void error(const char* message, int code) __attribute__ ((noreturn));
void custom_error(int code, const char* message, ...) __attribute__ ((noreturn));
char* freadline(FILE* f);
#endif 
