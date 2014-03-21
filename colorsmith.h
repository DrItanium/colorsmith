/* Instruction particle */
typedef unsigned char byte;
typedef unsigned short ushort;

typedef union ColorsmithInstruction {
   byte elements[2];
   struct {
      byte command : 2;
      /* overloaded modes :D */
      byte leg : 3; 
      byte ring : 3;
      byte intensity : 8;
   };
} ColorsmithInstruction;

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
