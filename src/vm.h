#ifndef vm_h
#define vm_h

#include "Bnuuy_chunk.h"
#include "Bnuuy_value.h"

// The stackmax is 256, just because.
#define STACK_MAX 256

typedef struct {
    Chunk* chunk;           //Bytecode chunk
    uint8_t* ip;            //Instruction pointer
    // STATE
    Value stack[STACK_MAX]; //Stack of values in the VM state
    Value* stackTop;        //Points to the start of the empty stack. 
    uint8_t line;           // A linenumber in byte, max 0XFF lines
} VM;

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR,
} InterpretResult;

//VM operations
void initVM();
void freeVM();

//Interprate code
InterpretResult interpret(const char* sourceCode);

// Stack operations
void push(Value value);
Value pop();

#endif