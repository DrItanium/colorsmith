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
int delayamount = 5;
int brightnesscap = 10;
int outward = 0;
int main(int argc, char* argv[]) {
   char* line;
   char* tmpline;
   FILE* file;
   int needsclosing, last, i, errorfree;
   FlowContainer container;
   long tmp0;
   container.cells = 0;
   container.count = 0;
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
                  fprintf(stderr, "couldn't open %s\n", line);
                  exit(errno);
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
         fprintf(stderr, "couldn't close %s\n", line); 
         exit(errno);
      }
   } else {
      usage(argv[0]);
   }
   return 0;
}

void usage(char* name) {
   printf("usage: %s [-d <delayamt>] [-b <brightnesscap>] [-u] <file>\n", name);
}

void decode(FlowContainer* container, FILE* input) {
   int a;
   byte output;
   a = fgetc(input);
   while(a != EOF) {
      output = temperbrightness(a);
      shiftcells(container, output);
      updateglow(container);
      if(delayamount) {
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
   count = LEDCount;
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
   for(count = container->count - 1; count > 0; count--) {
      ptr[count] = ptr[count - 1];
   }
   ptr[count] = value;
}
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
void updateglow(FlowContainer* container) {
   byte* ptr;
   int i, j;
   byte a, b;
   a = 0;
   b = 0;
   ptr = container->cells;
   for(i = 0; i < container->count; i++) {
      a = legarray[i];
      b = ringarray[i];
      piGlow1(a, b, ptr[i]);
      if(delayamount) {
         delay(delayamount);
      }
   }
}


void error(const char* message, int code) {
   fprintf(stderr, "%s\n", message);
   exit(code);
}
byte temperbrightness(int value) {
   return value % brightnesscap;
}
