#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <piGlow.h>
#include <string.h>
#include <errno.h>
#include "libcommon.h"
#include "piglowtranslationtable.h"


typedef struct FlowContainer {
   byte cells[LEDCount];
} FlowContainer;


/* inefficient but who cares */
void setup(FlowContainer* container);
void shutdown(FlowContainer* container);
void decode(FlowContainer* container, FILE* input);
void shiftcells(FlowContainer* container, byte value);
void updateglow(FlowContainer* container);

int delayamount = 5;
int brightnesscap = 10;
int outward = 0;

void usage(char* name);
void error(const char* message, int code);
byte temperbrightness(int input);
void commit(FlowContainer* container, byte brightness);

int main(int argc, char* argv[]) {
   char* line;
   char* tmpline;
   FILE* file;
   int needsclosing, last, i, errorfree;
   FlowContainer container;
   long tmp0;
   line = 0;
   file = 0;
   last = argc - 1;
   tmpline = 0;
   needsclosing = 0;
   errorfree = 1;
   tmp0 = 0L;
   if(argc > 1) {
      for(i = 1; errorfree && (i < last); ++i) {
         tmpline = argv[i];
         if(strlen(tmpline) == 2 && tmpline[0] == '-') {
            switch(tmpline[1]) {
               case 'b': 
                  {
                     ++i;
                     tmp0 = strtol(argv[i], NULL, 10);
                     if(errno == EINVAL) {
                        fprintf(stderr, "error: invalid brightness '%s' provided\n", argv[i]);
                        errorfree = 0;
                     } else if(errno == ERANGE) {
                        fprintf(stderr, "error: provided brightness: '%s'  is out of range\n", argv[i]);
                        errorfree = 0;
                     } else {
                        if(tmp0 < 0 || tmp0 > 255) {
                           fprintf(stderr, "error: provided brightness: %d is out of range\n", tmp0);
                           errorfree = 0;
                        } else {
                           brightnesscap = tmp0;
                        }
                     }
                     break;
                  }
               case 'd':
                  {
                     ++i;
                     tmp0 = strtol(argv[i], NULL, 10);
                     if(errno == EINVAL) {
                        fprintf(stderr, "error: invalid delay '%s' provided\n", argv[i]);
                        errorfree = 0;
                     } else if(errno == ERANGE) {
                        fprintf(stderr, "error: provided delay '%s' is out of range\n", argv[i]);
                        errorfree = 0;
                     } else {
                        if(tmp0 < 0) {
                           fprintf(stderr, "error: can't provide a negative delay\n");
                           errorfree = 0;
                        } else {
                           delayamount = tmp0;
                        }
                     }
                     break;

                  }
               case 'u':
                  outward = 1;
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
         if(i == last) {
            line = argv[last];
            if(strlen(line) == 1 && line[0] == '-') {
               file = stdin;
            } else if(strlen(line) >= 1 && line[0] != '-') {
               file = fopen(line, "r");
               if(!file) {
                  custom_error(errno, "couldn't open %s\n", line);
               }
               needsclosing = 1;
            }
         } else {
            fprintf(stderr, "no file provided\n");
         }
      }
   }

   if(file) {
      setup(&container);
      decode(&container, file);
      shutdown(&container);
      if(needsclosing && fclose(file) != 0) {
         custom_error(errno, "couldn't close %s\n", line);
      }
   } else {
      usage(argv[0]);
   }
   return 0;
}

void usage(char* name) {
   fprintf(stderr, "usage: %s [-d <delayamt>] [-b <brightnesscap>] [-u] <file>\n", name);
}

void decode(FlowContainer* container, FILE* input) {
   int a;
   byte output;
   a = fgetc(input);
   while(a != EOF) {
      output = temperbrightness(a);
      commit(container, output);
      a = fgetc(input);
   }
}

void setup(FlowContainer* container) {
   int i;
   piGlowSetup(0);
   for(i = 0; i < LEDCount; ++i) {
      container->cells[i] = 0;
   }
}
void shutdown(FlowContainer* container) {
   int i;
   /* turn off the leds by finishing the flow */
   for(i = 0; i < LEDCount; ++i) {
      commit(container, 0);
   }
}

void shiftcells(FlowContainer* container, byte value) {
   byte* ptr;
   int i;
   ptr = container->cells;
   for(i = LEDCount - 1; i > 0; i--) {
      ptr[i] = ptr[i - 1];
   }
   ptr[i] = value;
}
void updateglow(FlowContainer* container) {
   byte* ptr;
   int i, j;
   ptr = container->cells;
   for(i = 0; i < LEDCount; i++) {
      piGlow1(legarray[i], ringarray[i], ptr[i]);
      if(delayamount) {
         delay(delayamount);
      }
   }
}

void commit(FlowContainer* container, byte value) {
   shiftcells(container, value);
   updateglow(container);
   if(delayamount) {
      delay(delayamount);
   }
}

byte temperbrightness(int value) {
   return value % brightnesscap;
}
