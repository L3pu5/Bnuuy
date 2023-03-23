#include <stdlib.h>
#include "Bnuuy_memory.h"

void* reallocate (void* pointer, size_t oldSize, size_t newSize){
    //Deallocate if we want to request 0 size.
    if (newSize == 0){
        free(pointer);
    }

    // Call realloc otherwise.1
    void* result = realloc(pointer, newSize);
    //Crash ungracefully if we cannot allocate memory.
    if (result == NULL){
        exit(1);
    }
    
    return result;
}