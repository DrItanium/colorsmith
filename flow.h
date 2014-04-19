/* Instruction particle */
typedef unsigned char byte;
typedef unsigned short ushort;
enum {
   LEDCount = 18,
};

typedef struct FlowContainer {
   byte cells[LEDCount];
} FlowContainer;


/* inefficient but who cares */
void setup(FlowContainer* container);
void shutdown(FlowContainer* container);
void decode(FlowContainer* container, FILE* input);
void shiftcells(FlowContainer* container, byte value);
void updateglow(FlowContainer* container);
