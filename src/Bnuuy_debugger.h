#ifndef bnuuy_debugger_h
#define bnuuy_debugger_h

#include "Bnuuy_chunk.h"

void disassembleChunk(Chunk* chunk, const char* name);  // Read the chunk to end.
int disassembleInstruction(Chunk* chunk, int offset);  // Read instruction n bytes in/

#endif