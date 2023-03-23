#include <stdio.h>
#include "Bnuuy_common.h"
#include "Bnuuy_chunk.h"
#include "Bnuuy_debugger.h"
#include "vm.h"

int main(int argc, const char* argv[]){
    //Initialise the virtual machine
    initVM();
    // Make a Chunk
    Chunk chunk;
    startChunk(&chunk);

    // Disassembly/Learning
    int constant = addConstant(&chunk, 1.2);
    writeChunk(&chunk, OP_UPDATE_LINE);
    writeChunk(&chunk, 5);
    writeChunk(&chunk, OP_CONSTANT);
    writeChunk(&chunk, constant);
    writeChunk(&chunk, OP_RETURN);
    writeChunk(&chunk, OP_RETURN);
    writeChunk(&chunk, OP_RETURN);
    writeChunk(&chunk, OP_RETURN);
    disassembleChunk(&chunk, "test chunk");
    interpret(&chunk);
    // Free the virtual machine
    freeVM();
    freeChunk(&chunk);
    return 0;
}