#ifndef _TYPES_H
#define _TYPES_H
/* Instruction particle */
enum {
	LEDCount = 18,
	FieldCount = LEDCount + 1,
	RegisterCount = FieldCount,
	DelayRegister = RegisterCount - 1,
};

typedef unsigned char byte;
typedef unsigned short ushort;

void error(const char* message, int code);

byte legarray [] = {
   0, 0, 0, 0, 0, 0,
   1, 1, 1, 1, 1, 1,
   2, 2, 2, 2, 2, 2,
};
byte ringarray [] = {
   0, 1, 2, 3, 4, 5,
   0, 1, 2, 3, 4, 5,
   0, 1, 2, 3, 4, 5,
};

#endif 
