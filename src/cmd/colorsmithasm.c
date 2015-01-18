#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "libcommon.h"

void usage(char* name);
void decode(FILE* inputFile, FILE* outputFile);

int main(int argc, char* argv[]) {
   char* line;
   char* tmpline;
   char* outputpath;
   FILE* inputFile;
   FILE* outputFile;
   int needsclosing, last, i, errorfree;
   int outputFileNeedsClosing;
   line = 0;
   inputFile = 0;
   last = argc - 1;
   tmpline = 0;
   needsclosing = 0;
   errorfree = 1;
   outputFile = stdout;
   outputpath = 0;
   outputFileNeedsClosing = 0;
   if(argc > 1) {
      for(i = 1; errorfree && (i < last); ++i) {
         tmpline = argv[i];
         if(strlen(tmpline) == 2 && tmpline[0] == '-') {
            switch(tmpline[1]) {
               case 'o': 
                  {
                     // have an output file to write to
                     ++i;
                     outputpath = argv[i];
                     outputFile = fopen(outputpath, "w");
                     if(!outputFile) {
                        custom_error(errno, "couldn't open output file %s\n", outputpath);
                     }
                     outputFileNeedsClosing = 1;
                     break;
                  }

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
               inputFile = stdin;
            } else if(strlen(line) >= 1 && line[0] != '-') {
               inputFile = fopen(line, "r");
               if(!inputFile) {
                  custom_error(errno, "couldn't open %s\n", line);
               }
               needsclosing = 1;
            }
         } else {
            fprintf(stderr, "no file provided\n");
         }
      }
   }

   if(inputFile) {
      decode(inputFile, outputFile);
      if(needsclosing && fclose(inputFile) != 0) {
         custom_error(errno, "couldn't close %s\n", line);
      }
      if(outputFileNeedsClosing && fclose(outputFile) != 0) {
         custom_error(errno, "couldn't close %s\n", outputFile);
      }
   } else {
      usage(argv[0]);
   }
   return 0;
}

void usage(char* name) {
   fprintf(stderr, "usage: %s [-o <output-file>] <file>\n", name);
}

void decode(FILE* input, FILE* output) {
   ColorsmithMicroOperation uop;
   int result, lineNumber, i;
	char* line;
   result = 0;
   lineNumber = 1;
   i = 0;
	line = 0;
	uop_initialize(uop);
	do {
		line = freadline(input);
		if (strlen(line) > 0) {
			//fuck I hope this doesn't blow up in my face....
			result = sscanf(line, "%hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd %hhd", 
					&(uop[0]),  &(uop[1]),   &(uop[2]),   &(uop[3]),   &(uop[4]),  &(uop[5]), 
					&(uop[6]),  &(uop[7]),   &(uop[8]),   &(uop[9]),   &(uop[10]), &(uop[11]), 
					&(uop[12]), &(uop[13]),  &(uop[14]),  &(uop[15]),  &(uop[16]), &(uop[17]), 
					&(uop[18]));
			if (result == EOF || (result != FieldCount)) {
				free(line);
				line = 0;
				error("panic: Was given a file that couldn't be encoded properly!", errno);
			}
			uop_emit(uop, output);	
			free(line);
			line = 0;
		} else {
			free(line);
			line = 0;
		}
	} while (!feof(input) && !ferror(input));

}

