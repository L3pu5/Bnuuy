#include <stdlib.h>
#include "Bnuuy_chunk.h"
#include "Bnuuy_memory.h"
#include "Bnuuy_value.h"

void startChunk(Chunk* chunk){
    chunk->count        = 0;
    chunk->capacity     = 0;
    chunk->code         = NULL;
    chunk->line         = 0;
    initValueArray(&chunk->constants);
}

void writeChunk(Chunk* chunk, uint8_t byte){
    if(chunk->capacity < chunk->count + 1){
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY (oldCapacity);
        chunk->code     = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
    }
    chunk->code[chunk->count] = byte;
    chunk->count++;
}

/// @brief Returns the int index of the constant in the constants array.
/// @param chunk 
/// @param value 
/// @return The integer address of the location of the constant in the value array.
int addConstant(Chunk* chunk, Value value){
    writeValueArray(&chunk->constants, value);
    return chunk->constants.count - 1;
}

void freeChunk(Chunk* chunk){
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    freeValueArray(&chunk->constants);
    startChunk(chunk);
}