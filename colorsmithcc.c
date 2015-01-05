#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <piGlow.h>
#include <string.h>
#include <errno.h>
#include "types.h"

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
   RegisterCount = 19,
   DelayRegister = 18,
};

typedef struct colorsmithucodecore {
   byte registers[RegisterCount];
} colorsmithucodecore;


enum {
   ColorsmithCommandGlow1 = 0,
   ColorsmithCommandGlowLeg,
   ColorsmithCommandGlowRing,
   ColorsmithCommandDelay,
};

/* inefficient but who cares */
void core_initialize(colorsmithucodecore*);
void core_commitstate(colorsmithucodecore*, FILE*);
void decode(colorsmithucodecore* core, FILE* input, FILE* output);
void interpret(colorsmithucodecore* core, FILE* output, ColorsmithInstruction* inst);
void glow1(colorsmithucodecore* core, ColorsmithInstruction* inst);
void glowleg(colorsmithucodecore* core, ColorsmithInstruction* inst);
void glowring(colorsmithucodecore* core, ColorsmithInstruction* inst);
void glowdelay(colorsmithucodecore* core, ColorsmithInstruction* inst);
byte getnormalizedintensity(ColorsmithInstruction* inst);
byte getnormalizedring(ColorsmithInstruction* inst);
void usage(char* name);
void error(const char* message, int code);
void setleg(colorsmithucodecore* c, int leg, int intensity);
void setled(colorsmithucodecore* c, int leg, int ring, int intensity);
int parseBigEndian = 0;
int colorintensity = 10;
int allowdelay = 1;
/* read one byte at a time */
int bytemode = 0;
int main(int argc, char* argv[]) {
   colorsmithucodecore core;
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
      piGlowSetup(0);
      core_initialize(&core);
      decode(&core, file, outputfile);
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

void decode(colorsmithucodecore* core, FILE* input, FILE* output) {
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

void interpret(colorsmithucodecore* core, FILE* output, ColorsmithInstruction* inst) {
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
   core_commitstate(core, output);
   //we need to clear out the delay each iteration
   core->registers[DelayRegister] = 0;
}
/* this one is a little interesting since it is using a combination of leg and
 * ring to determine which individual led to set.
 *
 * This follows base 6 numbering so converting it to base 10 we can compute the
 * individual led as:
 *    index = 6 * leg + ring
 */
void glow1(colorsmithucodecore* core, ColorsmithInstruction* inst) {
   int ring, intensity;
   ring = getnormalizedring(inst);
   intensity = getnormalizedintensity(inst);
   switch(inst->leg) {
      case 0:
      case 1:
      case 2:
         setled(core, inst->leg, ring, intensity);
         break;
      case 3:
         setled(core, 0, ring, intensity);
         setled(core, 1, ring, intensity);
         break;
      case 4:
         setled(core, 0, ring, intensity);
         setled(core, 2, ring, intensity);
         break;
      case 5:
         setled(core, 1, ring, intensity);
         setled(core, 2, ring, intensity);
         break;
      case 6:
         // turn on all leds
         setled(core, 0, ring, intensity);
         setled(core, 1, ring, intensity);
         setled(core, 2, ring, intensity);
         break;
      case 7:
         /* turn them off */
         // just reinitialize the core in this case
         core_initialize(core);
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
void glowleg(colorsmithucodecore* core, ColorsmithInstruction* inst) {
   int intensity;
   intensity = getnormalizedintensity(inst);

   switch(inst->leg) {
      case 0:
      case 1:
      case 2:
         setleg(core, inst->leg, intensity);
         break;
      case 3:
         setleg(core, 0, intensity);
         setleg(core, 1, intensity);
         break;
      case 4:
         setleg(core, 0, intensity);
         setleg(core, 2, intensity);
         break;
      case 5:
         setleg(core, 1, intensity);
         setleg(core, 2, intensity);
         break;
      case 6:
         setleg(core, 0, intensity);
         setleg(core, 1, intensity);
         setleg(core, 2, intensity);
         break;
      case 7:
         /* turn them off */
         //just reinitialize the core since everything is being turned off
         core_initialize(core);
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
void glowring(colorsmithucodecore* core, ColorsmithInstruction* inst) {
   int ring, intensity;
   ring = getnormalizedring(inst);
   intensity = getnormalizedintensity(inst);
   switch(ring) {
      case 0:
         core->registers[0] = intensity;
         core->registers[6] = intensity;
         core->registers[12] = intensity;
         break;
      case 1:
         core->registers[1] = intensity;
         core->registers[7] = intensity;
         core->registers[13] = intensity;
         break;
      case 2:
         core->registers[2] = intensity;
         core->registers[8] = intensity;
         core->registers[14] = intensity;
         break;
      case 3:
         core->registers[3] = intensity;
         core->registers[9] = intensity;
         core->registers[15] = intensity;
         break;
      case 4:
         core->registers[4] = intensity;
         core->registers[10] = intensity;
         core->registers[16] = intensity;
         break;
      case 5:
         core->registers[5] = intensity;
         core->registers[11] = intensity;
         core->registers[17] = intensity;
         break;
      default:
         error("panic: unknown ring combo provided", 3);
   }
}
void glowdelay(colorsmithucodecore* core, ColorsmithInstruction* inst) {
   // keep the current state but just institute a delay. 
   if(allowdelay) {
      core->registers[DelayRegister] = inst->intensity;
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

void core_initialize(colorsmithucodecore* core) {
   /* initialize the registers to zero */
   int i;
   for(i = 0; i < RegisterCount; i++) {
      core->registers[i] = 0;
   }
}

void core_commitstate(colorsmithucodecore* core, FILE* file) {
   size_t result;
   result = fwrite(core, sizeof(colorsmithucodecore), 1, file);
   if (result != 1) {
      error("Couldn't output result to file", ferror(file));
   }
}

void setleg(colorsmithucodecore* c, int leg, int intensity) {
   int i, begin, end;
   switch (leg) {
      case 0:
         begin = 0;
         end = 6;
         break;
      case 1:
         begin = 6;
         end = 12;
         break;
      case 2:
         begin = 12;
         end = 18;
         break;
      default:
         error("panic: unknown leg provided to be set!", 4);
   }
   for(i = begin; i < end; i++) {
      c->registers[i] = intensity;
   }
}
void setled(colorsmithucodecore* c, int leg, int ring, int intensity) {
   // we end up with base 6 math since each leg has 6 entries and each ring has
   // three so we can come up with a base six math layout
   int index;
   index = (6 * leg) + ring;
   if (index >= 0 && index < 18) {
      c->registers[index] = intensity;
   } else {
      //print this and use lisp syntax because it is far more readable
      fprintf(stderr, "got illegal index %d from (+ (* 6 %d) %d)\n", index, leg, ring);
      error("panic: see above line!", 5);
   }
}
