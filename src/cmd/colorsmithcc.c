#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "libcommon.h"

/*
 * NOTE: this doesn't follow the internal piglow led layout but my own based
 * on how colorflow "animates" pixels. Since the piglow lays out leds in a
 * swirl, it is a little hard to describe which leds map to which led register.
 *
 * I will give a description as best as possible though.
 *
 * The Led closest to the piglow logo on the device is led zero. Following the
 * curve into the center, the next five leds are 1,2,3,4, and 5. Then starting
 * with the outmost led in the lower right corner is led 6. The next five leds
 * tracing into the center are 7,8,9,10,11. Then the final led leg starting in
 * the lower left hand corner is 12, 13, 14, 15,16, 17.
 *
 * Here is a crapy ascii art representation
 * ----------------------------------------
 * PIGLOW 
 *
 *               0
 *             1
 *           2
 *             3        10
 *               4   11    9
 *                 5         8
 *                   17        7
 *                16             6
 *    12       15
 *       13 14
 *
 * ----------------------------------------
 *
 * As I said, crappy representation but this is what I based everything off of.
 *
 */
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
void decode(ColorsmithMicroOperation, FILE*, FILE*);
void interpret(ColorsmithMicroOperation, FILE*, ColorsmithInstruction*);
void glow1(ColorsmithMicroOperation, ColorsmithInstruction*);
void glowleg(ColorsmithMicroOperation, ColorsmithInstruction*);
void glowring(ColorsmithMicroOperation, ColorsmithInstruction*);
void glowdelay(ColorsmithMicroOperation, ColorsmithInstruction*);
byte getnormalizedintensity(ColorsmithInstruction*);
byte getnormalizedring(ColorsmithInstruction*);
void usage(char* name);
int parseBigEndian = 0;
int colorintensity = 10;
int allowdelay = 1;
/* read one byte at a time */
int bytemode = 0;
int main(int argc, char* argv[]) {
   ColorsmithMicroOperation core;
   char* line;
   char* tmpLine;
   char* outputpath;
   FILE* file;
   FILE* outputfile;
   int needsClosing, last, i, errorFree, outputFileNeedsClosing;
   line = 0;
   file = 0;
   last = argc - 1;
   tmpLine = 0;
   needsClosing = 0;
   errorFree = 1;
   outputfile = stdout;
   outputpath = 0;
   outputFileNeedsClosing = 0;
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
               case 'o': 
                  {
                     // have an output file to write to
                     ++i;
                     outputpath = argv[i];
                     outputfile = fopen(outputpath, "w");
                     if(!outputfile) {
                        fprintf(stderr, "couldn't open output file %s\n", outputpath);
                        exit(errno);
                     }
                     outputFileNeedsClosing = 1;
                     break;
                  }
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
               fprintf(stderr, "couldn't open input %s\n", line);
               exit(errno);
            }
            needsClosing = 1;
         }
      }
   }

   if(file) {
		uop_initialize(core);
      decode(core, file, outputfile);
      if(needsClosing && fclose(file) != 0) {
         fprintf(stderr, "couldn't close %s\n", line); 
         exit(errno);
      }
      if (outputFileNeedsClosing && fclose(outputfile) != 0) {
         fprintf(stderr, "couldn't close %s\n", outputpath); 
         exit(errno);
      }
   } else {
      usage(argv[0]);
   }
   return 0;
}

void usage(char* name) {
   fprintf(stderr, "usage: %s [-o <outputfile>] [-b] [-f | -2] [-g] [-n] <file> \n", name);
}

void decode(ColorsmithMicroOperation core, FILE* input, FILE* output) {
   // translate 2 bytes to 19 bytes!
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
      interpret(core, output, &inst);
   } while(a != EOF && b != EOF);
}

void interpret(ColorsmithMicroOperation core, FILE* output, ColorsmithInstruction* inst) {
   switch(inst->command) {
      case ColorsmithCommandGlow1:
         glow1(core, inst);
         break;
      case ColorsmithCommandGlowLeg:
         glowleg(core, inst);
         break;
      case ColorsmithCommandGlowRing:
         glowring(core, inst);
         break;
      case ColorsmithCommandDelay:
         glowdelay(core, inst);
         break;
      default:
         error("panic: unknown command provided", 1);
   }
   //commit the core state now that we are done
   //default to stdout for now
	uop_emit(core, output);
   //we need to clear out the delay each iteration
	core[DelayRegister] = 0;
}
/* this one is a little interesting since it is using a combination of leg and
 * ring to determine which individual led to set.
 *
 * This follows base 6 numbering so converting it to base 10 we can compute the
 * individual led as:
 *    index = 6 * leg + ring
 */
void glow1(ColorsmithMicroOperation core, ColorsmithInstruction* inst) {
   int ring, intensity;
   ring = getnormalizedring(inst);
   intensity = getnormalizedintensity(inst);
   switch(inst->leg) {
      case 0:
      case 1:
      case 2:
         uop_setled(core, inst->leg, ring, intensity);
         break;
      case 3:
         uop_setled(core, 0, ring, intensity);
         uop_setled(core, 1, ring, intensity);
         break;
      case 4:
         uop_setled(core, 0, ring, intensity);
         uop_setled(core, 2, ring, intensity);
         break;
      case 5:
         uop_setled(core, 1, ring, intensity);
         uop_setled(core, 2, ring, intensity);
         break;
      case 6:
         // turn on all leds
         uop_setled(core, 0, ring, intensity);
         uop_setled(core, 1, ring, intensity);
         uop_setled(core, 2, ring, intensity);
         break;
      case 7:
         /* turn them off */
         // just reinitialize the core in this case
         uop_initialize(core);
         break;
      default:
         error("panic: unknown leg combo provided", 2);
   }
}
/* The following is the map for leg glowing to leds to be triggered:
 *
 * Leg 0: 0-5
 * Leg 1: 6-11
 * Leg 2: 12-17
 */
void glowleg(ColorsmithMicroOperation core, ColorsmithInstruction* inst) {
   int intensity;
   intensity = getnormalizedintensity(inst);

   switch(inst->leg) {
      case 0:
      case 1:
      case 2:
         uop_setleg(core, inst->leg, intensity);
         break;
      case 3:
         uop_setleg(core, 0, intensity);
         uop_setleg(core, 1, intensity);
         break;
      case 4:
         uop_setleg(core, 0, intensity);
         uop_setleg(core, 2, intensity);
         break;
      case 5:
         uop_setleg(core, 1, intensity);
         uop_setleg(core, 2, intensity);
         break;
      case 6:
         uop_setleg(core, 0, intensity);
         uop_setleg(core, 1, intensity);
         uop_setleg(core, 2, intensity);
         break;
      case 7:
         /* turn them off */
         //just reinitialize the core since everything is being turned off
         uop_initialize(core);
         break;
      default:
         error("panic: unknown leg combo provided", 2);
   }
}

/* According to the wiringPi comments for the piGlow, ring 0 is the outermost
 * and ring 5 is the inner most. Using the above guide, the layout of rings are
 * as follows:
 *
 * Ring 0: 0,  6, 12
 * Ring 1: 1,  7, 13
 * Ring 2: 2,  8, 14
 * Ring 3: 3,  9, 15
 * Ring 4: 4, 10, 16
 * Ring 5: 5, 11, 17
 */
void glowring(ColorsmithMicroOperation uop, ColorsmithInstruction* inst) {
   int ring, intensity;
   ring = getnormalizedring(inst);
   intensity = getnormalizedintensity(inst);
	uop_setring(uop, ring, intensity);
}
void glowdelay(ColorsmithMicroOperation uop, ColorsmithInstruction* inst) {
   // keep the current state but just institute a delay. 
   if(allowdelay) {
		uop[DelayRegister] = inst->intensity;
   }
}
byte getnormalizedintensity(ColorsmithInstruction* inst) {
   return inst->intensity % colorintensity;
}
byte getnormalizedring(ColorsmithInstruction* inst) {
   return inst->ring % 6;
}


