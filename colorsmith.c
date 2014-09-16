#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <piGlow.h>
#include <string.h>
#include <errno.h>
#include "types.h"


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
void usage(char* name);
void error(const char* message, int code);
int parseBigEndian = 0;
int colorintensity = 10;
int allowdelay = 1;
/* read one byte at a time */
int bytemode = 0;
int main(int argc, char* argv[]) {
   char* line;
   char* tmpLine;
   FILE* file;
   int needsClosing, last, i, errorFree;
   line = 0;
   file = 0;
   last = argc - 1;
   tmpLine = 0;
   needsClosing = 0;
   errorFree = 1;
   if(argc > 1) {
      for(i = 1; errorFree && (i < last); ++i) {
         tmpLine = argv[i];
         if(strlen(tmpLine) == 2 && tmpLine[0] == '-') {
            switch(tmpLine[1]) {
               case 'b':
                  parseBigEndian = 1;
                  break;
               case 'f':
                  colorintensity = 256;
                  printf("%s\n","WARNING: Do not stare directly at PiGlow LEDs in full intensity mode!\nThey are super bright");
                  break;
               case 'g':
                  bytemode = 1;
                  break;
               case '2':
                  colorintensity = 2;
                  break;
               case 'n':
                  allowdelay = 0;
                  break;
               default:
                  errorFree = 0;
                  break;
            }
         } else {
            errorFree = 0;
            break;
         }
      }
      if(errorFree) {
         line = argv[last];
         if(strlen(line) == 1 && line[0] == '-') {
            file = stdin;
         } else if(strlen(line) >= 1 && line[0] != '-') {
            file = fopen(line, "r");
            if(!file) {
               fprintf(stderr, "couldn't open %s\n", line);
               exit(errno);
            }
            needsClosing = 1;
         }
      }
   }

   if(file) {
      piGlowSetup(0);
      decode(file);
      if(needsClosing && fclose(file) != 0) {
         fprintf(stderr, "couldn't close %s\n", line); 
         exit(errno);
      }
   } else {
      usage(argv[0]);
   }
   return 0;
}

void usage(char* name) {
   fprintf(stderr, "usage: %s [-b] [-f | -2] [-g] [-n] <file> \n", name);
}

void decode(FILE* input) {
   ColorsmithInstruction inst;
   int a, b;
   do {
      a = fgetc(input);
      if(bytemode) {
         b = a;
      } else {
         b = fgetc(input);
      }

      if(parseBigEndian) {
         inst.command = getcommand(b);
         inst.leg = getleg(b);
         inst.ring = getring(b);
         inst.intensity = a;
      } else {
         inst.command = getcommand(a);
         inst.leg = getleg(a);
         inst.ring = getring(a);
         inst.intensity = b;
      }
      interpret(&inst);
   } while(a != EOF && b != EOF);
}

void interpret(ColorsmithInstruction* inst) {
   switch(inst->command) {
      case ColorsmithCommandGlow1:
         glow1(inst);
         break;
      case ColorsmithCommandGlowLeg:
         glowleg(inst);
         break;
      case ColorsmithCommandGlowRing:
         glowring(inst);
         break;
      case ColorsmithCommandDelay:
         glowdelay(inst);
         break;
      default:
         error("panic: unknown command provided", 1);
   }
}
void glow1(ColorsmithInstruction* inst) {
   int ring, intensity;
   ring = getnormalizedring(inst);
   intensity = getnormalizedintensity(inst);
   switch(inst->leg) {
      case 0:
      case 1:
      case 2:
         piGlow1(inst->leg, ring, intensity);
         break;
      case 3:
         piGlow1(0, ring, intensity);
         piGlow1(1, ring, intensity);
         break;
      case 4:
         piGlow1(0, ring, intensity);
         piGlow1(2, ring, intensity);
         break;
      case 5:
         piGlow1(1, ring, intensity);
         piGlow1(2, ring, intensity);
         break;
      case 6:
         piGlow1(0, ring, intensity);
         piGlow1(1, ring, intensity);
         piGlow1(2, ring, intensity);
         break;
      case 7:
         /* turn them off */
         piGlow1(0, ring, 0);
         piGlow1(1, ring, 0);
         piGlow1(2, ring, 0);
         break;
      default:
         error("panic: unknown leg combo provided", 2);
   }
}

void glowleg(ColorsmithInstruction* inst) {
   int intensity;
   intensity = getnormalizedintensity(inst);
   switch(inst->leg) {
      case 0:
      case 1:
      case 2:
         piGlowLeg(inst->leg, intensity);
         break;
      case 3:
         piGlowLeg(0, intensity);
         piGlowLeg(1, intensity);
         break;
      case 4:
         piGlowLeg(0, intensity);
         piGlowLeg(2, intensity);
         break;
      case 5:
         piGlowLeg(1, intensity);
         piGlowLeg(2, intensity);
         break;
      case 6:
         piGlowLeg(0, intensity);
         piGlowLeg(1, intensity);
         piGlowLeg(2, intensity);
         break;
      case 7:
         /* turn them off */
         piGlowLeg(0, 0);
         piGlowLeg(1, 0);
         piGlowLeg(2, 0);
         break;
      default:
         error("panic: unknown leg combo provided", 2);
   }
}

void glowring(ColorsmithInstruction* inst) {
   piGlowRing(getnormalizedring(inst), getnormalizedintensity(inst));
}

void glowdelay(ColorsmithInstruction* inst) {
   if(allowdelay) {
      delay(inst->intensity);
   }
}
byte getnormalizedintensity(ColorsmithInstruction* inst) {
   return inst->intensity % colorintensity;
}
byte getnormalizedring(ColorsmithInstruction* inst) {
   return inst->ring % 6;
}
void error(const char* message, int code) {
   fprintf(stderr, "%s\n", message);
   exit(code);
}
