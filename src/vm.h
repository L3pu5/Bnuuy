#ifndef vm_h
#define vm_h

#include "Bnuuy_chunk.h"

typedef struct {
    Chunk* chunk;       //Bytecode chunk
    uint8_t* ip;         //Instruction pointer
} VM;

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR,
} InterpretResult;

void initVM();
void freeVM();

#endif