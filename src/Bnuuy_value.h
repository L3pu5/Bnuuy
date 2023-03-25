#ifndef bnuuy_value_h
#define bnuuy_value_h

#include "Bnuuy_common.h"

// A value can no longer be a double, it was originally treated as a double entirely.
// We are going to create a type for it
//typedef double Value;

//Primitive Value types in clox
typedef enum {
    VAL_BOOL,
    VAL_NIL,
    VAL_NUMBER,
} ValueType;

//The struct implementing the 
typedef struct {
    ValueType type;
    //C-type unions are a struct element or element of memory which takes the size in memory of the largest member of the union.
    //This means that whatever is 'as' will have the length of double, even though boolean has one bit of data.
    // So comparisons need to consider this.
    //The name 'as' allows it to be easily dereferenced simiariliy as a cast, making it easier to use
    // Ie Value.as.boolean
    union { 
        //Largest element is Double = 8b
        // |00000000|00000000|00000000|00000000|00000000|00000000|00000000|00000000|
        // ^ This is size of object in memory, enum ValueType has size 4b as int
        // A 'variable' or constant in bnuuy currently observes 12b even for a 1 bit bool. (?)
        bool boolean;
        double number;
    } as;
} Value;

//Default 'constructors'/casters
#define BOOL_VAL(value)         ((Value){VAL_BOOL,      {.boolean = value}})
#define NIL_VAL                 ((Value){VAL_NIL,       {.number = 0}})
#define NUMBER_VAL(value)       ((Value){VAL_NUMBER,    {.number = value}})

//Default recasts/casts
#define AS_BOOL(value)          ((value).as.boolean)
#define AS_NUMBER(value)        ((value).as.number)

typedef struct {
    int capacity;
    int count;
    Value* values;
} ValueArray;

void initValueArray(ValueArray* array);
void writeValueArray(ValueArray* array, Value value);
void freeValueArray(ValueArray* array);
void printValue(Value value);

#endif