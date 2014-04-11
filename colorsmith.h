/* Instruction particle */
typedef unsigned char byte;
typedef unsigned short ushort;

typedef struct ColorsmithInstruction {
   byte command;
   byte leg;
   byte ring;
   byte intensity;
} ColorsmithInstruction;
#define getcommand(value) ((byte)(value & 0x3))
#define getleg(value) ((byte)((value & 0x1C) >> 2))
#define getring(value) ((byte)((value & 0xE0) >> 5))

enum {
   ColorsmithCommandGlow1 = 0,
   ColorsmithCommandGlowLeg,
   ColorsmithCommandGlowRing,
   ColorsmithCommandDelay,
};

/* inefficient but who cares */
void decode(FILE* input);
void interpret(ColorsmithInstruction* inst);
void glow1(ColorsmithInstruction* inst);
void glowleg(ColorsmithInstruction* inst);
void glowring(ColorsmithInstruction* inst);
void glowdelay(ColorsmithInstruction* inst);
byte getnormalizedintensity(ColorsmithInstruction* inst);
byte getnormalizedring(ColorsmithInstruction* inst);
