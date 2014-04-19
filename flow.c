#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <piGlow.h>
#include <string.h>
#include <errno.h>
#include "flow.h"

void usage(char* name);
void error(const char* message, int code);
byte temperbrightness(int input);
int legmode = 0;
int ringmode = 0;
int allowdelay = 1;
int delayamount = 5;
int brightnesscap = 10;
int outward = 0;
int main(int argc, char* argv[]) {
   char* line;
   char* tmpline;
   FILE* file;
   int needsclosing, last, i, errorfree;
   FlowContainer container;
   container.cells = 0;
   container.count = 0;
   line = 0;
   file = 0;
   last = argc - 1;
   tmpline = 0;
   needsclosing = 0;
   errorfree = 1;
   if(argc > 1) {
      for(i = 1; errorfree && (i < last); ++i) {
         tmpline = argv[i];
         if(strlen(tmpline) == 2 && tmpline[0] == '-') {
            switch(tmpline[1]) {
               case 'l':
                  if(ringmode == 0) {
                     legmode = 1;
                  } else {
                     fprintf(stderr, "error: %s\n", "can't do both leg and ring mode");
                     errorfree = 0;
                  }
                  break;
               case 'r':
                  if(legmode == 0) {
                     ringmode = 1;
                  } else {
                     fprintf(stderr, "error: %s\n", "can't do both leg and ring mode");
                     errorfree = 0;
                  }
                  break;
               case 'u':
                  outward = 1;
                  break;
               case 'n':
                  allowdelay = 0;
                  break;
               default:
                  errorfree = 0;
                  break;
            }
         } else {
            errorfree = 0;
            break;
         }
      }
      if(errorfree) {
         line = argv[last];
         if(strlen(line) == 1 && line[0] == '-') {
            file = stdin;
         } else if(strlen(line) >= 1 && line[0] != '-') {
            file = fopen(line, "r");
            if(!file) {
               fprintf(stderr, "couldn't open %s\n", line);
               exit(errno);
            }
            needsclosing = 1;
         }
      }
   }

   if(file) {
      setup(&container);
      decode(&container, file);
      shutdown(&container);
      if(needsclosing && fclose(file) != 0) {
         fprintf(stderr, "couldn't close %s\n", line); 
         exit(errno);
      }
   } else {
      usage(argv[0]);
   }
   return 0;
}

void usage(char* name) {
   printf("usage: %s [-l | -r] [-u] [-n] <file>\n", name);
}

void decode(FlowContainer* container, FILE* input) {
   int a;
   byte output;
   a = fgetc(input);
   while(a != EOF) {
      output = temperbrightness(a);
      shiftcells(container, output);
      updateglow(container);
      if(allowdelay) {
         delay(delayamount);
      }
      a = fgetc(input);
   }
}

void setup(FlowContainer* container) {
   byte count;
   byte* tmp;
   piGlowSetup(0);
   tmp = 0;
   count = 0;
   if(legmode) {
      count = LegCount;
   } else if(ringmode) {
      count = ringmode;
   } else {
      count = LEDCount;
   }
   tmp = calloc(count + 1, sizeof(byte));
   if(tmp) {
      container->cells = tmp;
      container->count = count;
   } else {
      error("couldn't allocate memory!", errno);
   }
}
void shutdown(FlowContainer* container) {
   free(container->cells);
}

void shiftcells(FlowContainer* container, byte value) {
   byte* ptr;
   int count;
   count = 0;
   ptr = container->cells;
   if(legmode) {
      ptr[2] = ptr[1];
      ptr[1] = ptr[0];
      ptr[0] = value;
   } else if(ringmode) {
      ptr[5] = ptr[4];
      ptr[4] = ptr[3];
      ptr[3] = ptr[2];
      ptr[2] = ptr[1];
      ptr[1] = ptr[0];
      ptr[0] = value;
   } else {
      for(count = container->count - 1; count > 0; count--) {
         ptr[count] = ptr[count - 1];
      }
      ptr[count] = value;
   }
}
void updateglow(FlowContainer* container) {
#define trydelay if(allowdelay) delay(delayamount)

   byte* ptr;
   int i;
   byte a, b;
   a = 0;
   b = 0;
   ptr = container->cells;
   if(legmode) {
      piGlowLeg(0, ptr[0]);
      trydelay;
      piGlowLeg(1, ptr[1]);
      trydelay;
      piGlowLeg(2, ptr[2]);
      trydelay;
   } else if(ringmode) {
      if(outward) {

         piGlowRing(5, ptr[0]);
         trydelay;
         piGlowRing(4, ptr[1]);
         trydelay;
         piGlowRing(3, ptr[2]);
         trydelay;
         piGlowRing(2, ptr[3]);
         trydelay;
         piGlowRing(1, ptr[4]);
         trydelay;
         piGlowRing(0, ptr[5]);
         trydelay;
      } else {
         piGlowRing(0, ptr[0]);
         trydelay;
         piGlowRing(1, ptr[1]);
         trydelay;
         piGlowRing(2, ptr[2]);
         trydelay;
         piGlowRing(3, ptr[3]);
         trydelay;
         piGlowRing(4, ptr[4]);
         trydelay;
         piGlowRing(5, ptr[5]);
         trydelay;
      }
   } else {
      for(i = 0; i < container->count; i++) {
         if(i < 6) {
            a = 0;
         } else if(i >= 6 && i < 12) {
            a = 1;
         } else {
            a = 2;
         }
         if(outward) {
            b = 6 - ((i % 6) + 1);
         } else {
            b = i % 6;
         }
         piGlow1(a, b, ptr[i]);
         trydelay;
      }
   }
#undef trydelay
}


void error(const char* message, int code) {
   fprintf(stderr, "%s\n", message);
   exit(code);
}
byte temperbrightness(int value) {
   return value % brightnesscap;
}
