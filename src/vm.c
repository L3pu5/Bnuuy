#include <stdio.h>

#include "Bnuuy_common.h"
#include "Bnuuy_chunk.h"
#include "Bnuuy_debugger.h"
#include "compiler.h"
#include "vm.h"

// Have a static VM ready to go.
VM vm;

static void resetStack(){
    vm.stackTop = vm.stack;
}
void initVM(){
    resetStack();
}

void freeVM(){
    resetStack();
}

void push(Value value){
    //Set the element at this position 
    *vm.stackTop = value;
    //Incremenet the pointer
    vm.stackTop++;
}

Value pop(){
    //Regress the pointer (we are 1 ahead)
    vm.stackTop--;
    return *vm.stackTop;
}

//This is the program.
// The virtual machine reads bytes from the chunk and
// 'dispatches' or 'decodes' them to the C implementation of the code.

static InterpretResult run(){
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
//Use a macro to define tedious repeatable chunks of code in C
#define BINARY_OP(op) \
    do {\
        Value b = pop();\
        Value a = pop();\
        push(a op b);\
    } while (false)\

//Definition for local scope to advance instruction pointer.
    for (;;) {
        //If we are in DEBUG mode, disassemble the instruction.
        #ifdef DEBUG_TRACE_EXECUTION
        //Dump the stack
        printf("            ");
        for(Value* slot = vm.stack; slot < vm.stackTop; slot++){
            printf("[");
            printValue(*slot);
            printf("]");
        }
        printf("\n");
        //Dissassemble the chunk
        disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
        #endif

        uint8_t instruction;    // Byte under the pointer for the bytecode.

        switch (instruction = READ_BYTE()){
            //Binary arithmetic operations
            case OP_ADD:                BINARY_OP(+); break;
            case OP_SUBTRACT:           BINARY_OP(-); break;
            case OP_MULTIPLY:           BINARY_OP(*); break;
            case OP_DIVIDE:             BINARY_OP(/); break;

            //Unary operation, negate a variable on the stack
            case OP_NEGATE: {
                push(-pop());
                break;
            }
            // Update line bytecode
            case OP_UPDATE_LINE: {
                uint8_t line = READ_BYTE();
                vm.line = line;
                break;
            }
            //For a constant bytecode we read the Constant and for now we will print it.
            case OP_CONSTANT: {
                //printf("OP_CONSTANT");
                Value constant = READ_CONSTANT();
                push(constant);
                break;
            }
            //If we make it to return without throwing an error we intepreted okay!
            case OP_RETURN: {
                //Pop the stack
                printValue(pop());
                printf("\n");
                return INTERPRET_OK;
            }
            default:
                printf("Unexpected instruction.");
                return INTERPRET_COMPILE_ERROR;
        }
    }
#undef READ_BYTE
#undef READ_CONSTANT
}


InterpretResult interpret(const char* source){
    Chunk chunk;
    startChunk(&chunk);

    //Compiler takes a source, exports it to a chunk to feed to VM.
    if(!compile(source, &chunk)){
        freeChunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }
    
    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    InterpretResult result = run();
    freeChunk(&chunk);
    //compile(source);
    return result;
}