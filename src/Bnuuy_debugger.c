#include <stdlib.h>
#include <stdio.h>

#include "Bnuuy_debugger.h"
#include "Bnuuy_value.h"

void disassembleChunk(Chunk* chunk, const char* name){
    printf("Chunk: %s\n", name);
    for (size_t offset = 0; offset < chunk->count;)
    {
        offset = disassembleInstruction(chunk, offset);
    }
}

static int simpleInstruction(const char* name, int offset){
    printf("%s\n", name);
    return offset + 1;
}

static int constantInstruction(const char* name, Chunk* chunk, int offset){
    //The constant address is a single byte. 
    // At this point the clox VM only supports one byte (up to xFF) constants. 
    uint8_t constant = chunk->code[offset + 1];
    printf("%-16s %4d '", name, constant);
    printValue(chunk->constants.values[constant]);
    printf("\n");
    return offset + 2;
}

// With this implementation, clox only allows up to 0xFF lines of code
static int updateLineInstruction(const char* name, Chunk* chunk, int offset ){
    //Grab the next byte of code which is the line number.
    uint8_t line = chunk->code[offset + 1];
    chunk->line = line;
    printf("%-16s %4d '", name, line);
    printf("%u", line);
    printf("\n");
    return offset + 2;
}

int disassembleInstruction(Chunk* chunk, int offset){
    printf("%04d ", offset);
    printf("%03u ", chunk->line);
    uint8_t instruction = chunk->code[offset];
    switch(instruction){
        case OP_UPDATE_LINE:
            return updateLineInstruction("OP_UPDATE_LINE", chunk, offset);
        case OP_CONSTANT:
            return constantInstruction("OP_CONSTANT", chunk, offset);
        case OP_RETURN:
            return simpleInstruction("OP_RETURN", offset);
        default:
            printf("Unknown OpCode %d\n", instruction);
            return offset + 1;
    }
}

