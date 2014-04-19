/* Instruction particle */
typedef unsigned char byte;
typedef unsigned short ushort;
enum {
   LegCount = 3,
   LEDCount = 18,
   RingCount = 6,
};
typedef struct FlowContainer {
   byte* cells;
   byte count;
} FlowContainer;


/* inefficient but who cares */
void setup(FlowContainer* container);
void shutdown(FlowContainer* container);
void decode(FlowContainer* container, FILE* input);
void shiftcells(FlowContainer* container, byte value);
void updateglow(FlowContainer* container);
