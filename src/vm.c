#include "Bnuuy_common.h"
#include "vm.h"

// Have a static VM readt to go.
VM vm;

void initVM(){

}

void freeVM(){

}

InterpretResult interpret(Chunk* chunk){
    vm.chunk = chunk;
    vm.ip = vm.chunk->code;
    return run();
}

//This is the program.
// The virtual machine reads bytes from the chunk and
// 'dispatches' or 'decodes' them to the C implementation of the code.

static InterpretResult run(){
#define READ_BYTE() (*vm.ip++)
//Definition for local scope to advance instruction pointer.
    for (;;) {
        uint8_t instruction;    // Byte under the pointer
        switch (instruction = READ_BYTE()){
            case OP_RETURN: {
                return INTERPRET_OK;
            }
        }
    }
}