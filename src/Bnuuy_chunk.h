#ifndef bnuuy_chunk_h
#define bnuuy_chunk_h

#include "Bnuuy_common.h"
#include "Bnuuy_value.h"

//OPCODES with parameters can inject data into the stream immediately after the opcode
typedef enum {
    OP_UPDATE_LINE,
    OP_CONSTANT,
    OP_RETURN,
} OpCode;

// Chunk
// Dynmaically growing array of bytes.
typedef struct {
    int count;              // Number of elements currently stored.
    int capacity;           // Number of elements drafted in space.
    int line;               // The current "line" of code the interpreter is parsing
    uint8_t* code;          // 1 byte unsigned integer.
    ValueArray constants;   // We take the constants with the chunk
} Chunk;

void startChunk(Chunk* chunk);                  // Initialise a new chunk.
void writeChunk(Chunk* chunk, uint8_t byte);    // Write a single byte to the chunk
void freeChunk(Chunk* chunk);                   // Requests the freeing of chunk memory
int  addConstant(Chunk* chunk, Value value);    // This writes a constant Value to the chunk.
   

#endif