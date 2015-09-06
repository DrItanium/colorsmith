#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "libcommon.h"

/* inefficient but who cares */
int cycleCount(void);
void setup(void);
void setupThird(void);
void shutdown(byte* container);
void decode(byte* container, FILE* input);
void shiftcells(byte* container, byte value);
void updateglow(byte* container);
void updateglowThird(byte* container);

typedef void (*Starter)(void);
typedef void (*ContainerOp)(byte* container);
int delayamount = 5;
int brightnesscap = 10;
int numLEDs = LEDCount;
FILE* outputFile;
// the variable aspects for different modes
Starter setupLambda = setup;
ContainerOp updateglowLambda = updateglow;

void usage(char* name);
void error(const char* message, int code);
byte temperbrightness(int input);
void commit(byte* container, byte brightness);

int main(int argc, char* argv[]) {
	char* line;
	char* tmpline;
	char* outputpath;
	FILE* file;
	int needsclosing, last, i, errorfree;
	int outputFileNeedsClosing;
	byte* cells = 0;
	long tmp0;
	line = 0;
	file = 0;
	last = argc - 1;
	tmpline = 0;
	needsclosing = 0;
	errorfree = 1;
	tmp0 = 0L;
	outputFile = stdout;
	outputpath = 0;
	outputFileNeedsClosing = 0;
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
					case 't':
						numLEDs = LEDCount / 3; // we only have 6 bytes to work with in this mode
						updateglowLambda = updateglowThird;
						setupLambda = setupThird;
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
		cells = malloc(numLEDs * sizeof(byte));
		if (cells == NULL) {
			custom_error(32, "Couldn't allocate %d bytes!\n", numLEDs);
		}
		setupLambda();
		decode(cells, file);
		shutdown(cells);
		free(cells);
		cells = 0;
		if(needsclosing && fclose(file) != 0) {
			custom_error(errno, "couldn't close %s\n", line);
		}
		if(outputFileNeedsClosing && fclose(outputFile) != 0) {
			custom_error(errno, "couldn't close %s\n", outputpath);
		}
	} else {
		usage(argv[0]);
	}
	return 0;
}

void usage(char* name) {
	fprintf(stderr, "usage: %s [-o <output-file>] [-d <delayamt>] [-b <brightnesscap>] [-t] <file>\n", name);
}
// common functions
void shutdown(byte* container) {
	/* turn off the leds by finishing the flow */
	for(int i = 0; i < numLEDs; ++i) {
		commit(container, 0);
	}
}

void shiftcells(byte* container, byte value) {
	int i;
	byte* ptr = container;
	for(i = numLEDs - 1; i > 0; i--) {
		ptr[i] = ptr[i - 1];
	}
	ptr[i] = value;
}

void commit(byte* container, byte value) {
	shiftcells(container, value);
	// Used to provide even more of a delay without resorting to tuning hacks
	// via the delay value. It seems that the space blowup for these operations
	// is even higher than I thought. This provides the correct amount of
	// delay to make it look pretty constant. One loop will always be invoked
	for (int i = 0; i < cycleCount(); i++) {
		updateglowLambda(container);
	}
}
int cycleCount() {
	return delayamount + 1;
}

byte temperbrightness(int value) {
	return value % brightnesscap;
}

void decode(byte* container, FILE* input) {
	for (int a = fgetc(input); a != EOF; a = fgetc(input)) {
		commit(container, temperbrightness(a));
	}
}

// fullwidth

void setup() {
	ColorsmithMicroOperation uop;
	uop_initialize(uop);
	uop_emit(uop, outputFile);
}


void updateglow(byte* container) {
	byte* ptr = container;
	ColorsmithMicroOperation uop;
	int i;
	uop_initialize(uop);
	ptr = container;
	for(i = 0; i < LEDCount; i++) {
		uop[i] = ptr[i];
	}
	uop[i] = delayamount;
	uop_emit(uop, outputFile);
}

// third mode or leg mode
void setupThird() {
	byte data[LegFieldCount];
	initialize(data, LegFieldCount);
	emit(data, LegFieldCount, outputFile);
}
void updateglowThird(byte* container) {
	int i;
	byte data[LegFieldCount];
	initialize(data, LegFieldCount);
	for (i = 0; i < LegLEDCount; i++) {
		data[i] = container[i];
	}
	data[i] = delayamount;
	emit(data, LegFieldCount, outputFile);
}
