#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// const char* cstring = "asdasdasd";

// int unwrap_string(const char* ptr, int max_depth){
//     size_t index = 0;
//     while(index < max_depth){
//         char* token = (ptr + (sizeof(char) * index));
//         printf("%u : ", token);
//         printf("%c - %i (%i)\n", *token, *token, (*token == '\0'));
//         if( *token == '\0'){
//             printf("We escaped the string. A null character at %i\n", index);
//             return index;
//         }
//         index++;
//     }
//     return -1;
//}
    // printf("%s has length %i\n", cstring, strlen(cstring));
    // unwrap_string(cstring, 10);

typedef struct Point{
    char name[12];
    int x;
    int y;
} Point;


int main(){
    char c = 'a';
    if('0' < c  && c < '9') printf("C is a digit.");
    Point P1;// = (struct Point*) malloc(sizeof(struct Point));
    strcpy(P1.name, "TESTTESTESTTEST");
    printf("%u, %u, %u \n", sizeof(P1), sizeof(struct Point), sizeof(Point));
    printf("%s - %i\n", P1.name, strlen(P1.name));
    printf("%p, %p", P1.name, &P1.name);
    return 0;
}
