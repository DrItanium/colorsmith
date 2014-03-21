#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <piGlow.h>
#include "colorsmith.h"

int main(int argc, char* argv[]) {
   piGlowSetup(0);
   decode(stdin);
   return 0;
}

void decode(FILE* input) {
   ColorsmithInstruction inst;
   int value;
   do {
     value = fgetc(input);
     inst.elements[0] = value;
     value = fgetc(input);
     inst.elements[1] = value;
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
   int ring;
   ring = inst->ring % 6;
   switch(inst->leg) {
      case 0:
      case 1:
      case 2:
         piGlow1(inst->leg, ring, inst->intensity);
         break;
      case 3:
         piGlow1(0, ring, inst->intensity);
         piGlow1(1, ring, inst->intensity);
         break;
      case 4:
         piGlow1(0, ring, inst->intensity);
         piGlow1(2, ring, inst->intensity);
         break;
      case 5:
         piGlow1(1, ring, inst->intensity);
         piGlow1(2, ring, inst->intensity);
         break;
      case 6:
         piGlow1(0, ring, inst->intensity);
         piGlow1(1, ring, inst->intensity);
         piGlow1(2, ring, inst->intensity);
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
   switch(inst->leg) {
      case 0:
      case 1:
      case 2:
         piGlowLeg(inst->leg, inst->intensity);
         break;
      case 3:
         piGlowLeg(0, inst->intensity);
         piGlowLeg(1, inst->intensity);
         break;
      case 4:
         piGlowLeg(0, inst->intensity);
         piGlowLeg(2, inst->intensity);
         break;
      case 5:
         piGlowLeg(1, inst->intensity);
         piGlowLeg(2, inst->intensity);
         break;
      case 6:
         piGlowLeg(0, inst->intensity);
         piGlowLeg(1, inst->intensity);
         piGlowLeg(2, inst->intensity);
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
   piGlowRing(inst->ring % 6, inst->intensity);
}

void glowdelay(ColorsmithInstruction* inst) {
   union {
      ushort value;
      struct {
         byte leg : 3;
         byte ring : 3;
         byte intensity : 8;
      };
   } container;
   container.leg = inst->leg;
   container.ring = inst->ring;
   container.intensity = inst->intensity;
   delay(container.value);
}
