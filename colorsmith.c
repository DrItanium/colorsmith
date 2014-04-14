#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <piGlow.h>
#include <string.h>
#include <errno.h>
#include "colorsmith.h"

void usage(char* name);
int main(int argc, char* argv[]) {
   char* line;
   FILE* file;
   int needsClosing;
   line = 0;
   file = 0;
   needsClosing = 0;
   piGlowSetup(0);
   if(argc == 2) {
      line = argv[1];
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
   if(file) {
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
   printf("usage: %s <file> | - \n", name);
}

void decode(FILE* input) {
   ColorsmithInstruction inst;
   int value;
   do {
      value = fgetc(input);
      inst.command = getcommand(value);
      inst.leg = getleg(value);
      inst.ring = getring(value);
      inst.intensity = fgetc(input);
      interpret(&inst);
   } while(value != EOF);
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
         fprintf(stderr, "%s\n", "panic: unknown command provided");
         exit(1);
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
         fprintf(stderr, "%s\n", "panic: unknown leg combo provided");
         exit(2);
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
         fprintf(stderr, "%s\n", "panic: unknown leg combo provided");
         exit(2);
   }
}

void glowring(ColorsmithInstruction* inst) {
   piGlowRing(getnormalizedring(inst), getnormalizedintensity(inst));
}

void glowdelay(ColorsmithInstruction* inst) {
   delay(inst->intensity);
}
byte getnormalizedintensity(ColorsmithInstruction* inst) {
   return inst->intensity % 10;
}
byte getnormalizedring(ColorsmithInstruction* inst) {
   return inst->ring % 6;
}
