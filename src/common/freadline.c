#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include "libcommon.h"

enum {
	CapacityIncreaseSize = 80,
	EndConstants
};
char* freadline(FILE* f) {
	char* output;
	int curr, count, capacity;
	curr = 0;
	output = NULL;
	if (f && !feof(f)) {
		count = 0;
		capacity = CapacityIncreaseSize;
		output = calloc(CapacityIncreaseSize, sizeof(char));
		for (curr = fgetc(f); curr != '\n' && curr != EOF; curr = fgetc(f)) {
			if (count == capacity) {
				capacity += CapacityIncreaseSize;
				output = realloc(output, capacity);
				if (output == NULL) {
					break;
				} 
			}
			output[count] = curr;
			count++;
		}
		if (count == capacity) {
			output = realloc(output, capacity+1);
			if (output != NULL) {
				output[count+1] = 0;
			}
		}
	}
	return output;
}
