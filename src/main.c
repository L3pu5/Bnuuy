#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Bnuuy_common.h"
#include "Bnuuy_chunk.h"
#include "Bnuuy_debugger.h"
#include "vm.h"

static void repl() {
    //1024 character line buffer
    char line[1024];
    for (;;){
        printf("> ");

        if(!fgets(line, 1024, stdin)){
            printf("\n");
            break;
        }
        interpret(line);
    }
}

static char* readFile(const char* path){
    //Open the file
    FILE* file = fopen(path, "rb");
    
    //Check for null file
    if(file == NULL){
        fprintf(stderr, "Couldn't open file at %s", path);
        exit(74);
    }
    fseek(file, 0L, SEEK_END); //Seek the end of the file.
    size_t fileSize = ftell(file); // Grab the end byte
    rewind(file);                   // Return to the start of the file

    char* buffer = (char*)  malloc(fileSize + 1);
    if(buffer == NULL){
        fprintf(stderr, "Couldn't assign memory of size %li", fileSize);
        exit(74);
    }

    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    buffer[bytesRead] = '\0'; // Add null byte

    fclose(file);
    return buffer;
}



static void runFile(const char* path){
    char* sourceCode = readFile(path);
    InterpretResult result = interpret(sourceCode);
    free(sourceCode);

    if(result == INTERPRET_RUNTIME_ERROR) exit(65);
    if(result == INTERPRET_COMPILE_ERROR) exit(70);
}

int main(int argc, const char* argv[]){
    //Initialise the virtual machine
    initVM();
    if(argc == 1){
        //Drop into a repl 
        repl();
    } else if (argc == 2){
        //Run a file
        runFile(argv[1]);
    } else{
        fprintf(stderr, "Usage: bnuuy [path]\n");
    }

    //Free the virtual machine
    freeVM();
    return 0;
}