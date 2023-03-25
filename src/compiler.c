#include <stdlib.h>
#include <stdio.h>

#include "bnuuy_common.h"
#include "compiler.h"
#include "scanner.h"

#ifdef DEBUG_PRINT_CODE
#include "Bnuuy_debugger.h"
#endif



typedef struct {
    Token   current;
    Token   previous;
    bool    hadError;
    bool    panicMode; //We open panicmode when we encounter an error. This will stop the compiler from throwing additional errors after the first and causing grief to the writer.
} Parser;

typedef enum {
    PREC_NONE,
    PREC_ASSIGNMENT,    // =
    PREC_OR,            // |
    PREC_AND,           // &
    PREC_EQUALITY,      // == !=
    PREC_COMPARISON,    // > <
    PREC_TERM,          // + -
    PREC_FACTOR,        // * /
    PREC_UNARY,         // ! -
    PREC_CALL,          // . ()
    PREC_PRIMARY,       // 
} Precedence;

typedef void (*ParseFn)();
typedef struct {
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
} ParseRule;

Parser parser;
Chunk* compilingChunk;

static Chunk* currentChunk(){
    return compilingChunk;
}

static void errorAt(Token* token, const char* error_message){
    fprintf(stderr, "[line %d] Error", token->line);

    if(token->type == TOKEN_EOF){
        fprintf(stderr, " at end.");
    } else if(token->type == TOKEN_ERROR) {
        // Don't report
    } else {
        fprintf(stderr, "at '%.*s'", token->length, token->start);
    }

    fprintf(stderr, ": '%s\n", error_message);
    parser.hadError = true;
}

static void error(const char* error_message){
    errorAt(&parser.previous, error_message);
}

//Remember ErrorToken has a error string and not a set string
static void errorAtCurrent(const char* error_message){
    errorAt(&parser.current, error_message);
}


static void advance(){
    parser.previous = parser.current;
    for (;;) {
        parser.current = scanToken();
        if (parser.current.type != TOKEN_ERROR) break;

        errorAtCurrent(parser.current.start);
    }
}

//Check to see if the next token is the one we expect, otherwise throw an error.
static void consume(TokenType type, const char* message){
    if(parser.current.type == type){
        advance();
        return;
    }

    errorAtCurrent(message);
}

//Print out the bytecode.
static void emitByte(uint8_t byte){
    writeChunk(currentChunk(), byte);
}

//Helper function for multiple bytes
static void emitBytes(uint8_t byte1, uint8_t byte2){
    emitByte(byte1);
    emitByte(byte2);
}

static int makeConstant(Value constantValue){
    int constant = addConstant(currentChunk(), constantValue);
    if(constant > UINT8_MAX){
        error("Too many constants in the chunk.");
        return 0;
    }
    return (uint8_t) constant;
}

static void emitConstant(Value constantValue){
    emitBytes(OP_CONSTANT, makeConstant(constantValue));
}

static void emitReturn(){
    emitByte(OP_RETURN);
}

static void endCompiler(){
    emitReturn();
#ifdef DEBUG_PRINT_CODE
    //If we haven't had an error, disassemble the chunk
    if(!parser.hadError){
        disassembleChunk(currentChunk(), "code");
    }
#endif
}


//          PRATT PARSER

static void expression();
static ParseRule* getRule(TokenType type);
static void parsePrecedence(Precedence precedence);



static void binary(){
    TokenType operatorType = parser.previous.type;
    //Determine which precedence is appropriate 
    // Are we adding, dividing, something else?
    ParseRule* rule = getRule(operatorType);
    //We recursively read ahead to grab the above potential operators which are more important than us. IE, the next term (a, b, c) and any unary or grouping expressions. This way 3 + (a + b) from '+' GRABS () which GRABS a + which GRABS b and each of these are pushed onto, then popped from the stack.
    parsePrecedence((Precedence) (rule->precedence+1));

    switch(operatorType){
        case TOKEN_PLUS:            emitByte(OP_ADD); break;
        case TOKEN_MINUS:           emitByte(OP_SUBTRACT); break;
        case TOKEN_SLASH:           emitByte(OP_DIVIDE); break;
        case TOKEN_STAR:            emitByte(OP_MULTIPLY); break;
        default:                    return; //unreachable
    }
}


// EXPRESSIONS. STUDY THIS BAD BOY. Pratt's algorithm?
//Each token has its own type of expression.
//Each expresion has a function that spits out bytecode.
// We build an array of function pointers with indexes aligning to the TokenTypes.
static void expression() {
    //An expression compiles everything from the lowest level upwards.
    parsePrecedence(PREC_ASSIGNMENT);
}

static void unary() {
    TokenType operatorType = parser.previous.type;
    
    //Compile the operand ie: we can have -(1+2)
    parsePrecedence(PREC_UNARY);

    //Compile the expression to bytecode
    switch(operatorType){
        case TOKEN_MINUS: emitByte(OP_NEGATE); break;
        default: return; //Unreachable
    }
}

static void grouping() {
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after an expression to end a grouping.");
}

//Number expression
static void number() {
    double value = strtod(parser.previous.start, NULL);
    emitConstant(value);
}


// PARSE RULES. 
// EACH TOKENTYPE has a set of RULES determining how to HANDLE it based on if it is an infix, postfix, prefix operator. It points to the instruction to compile the instruction.
ParseRule rules[] = {
    [TOKEN_LEFT_PAREN]      = {grouping,    NULL,       PREC_NONE},
    [TOKEN_RIGHT_PAREN]     = {NULL,        NULL,       PREC_NONE},
    [TOKEN_LEFT_BRACE]      = {NULL,        NULL,       PREC_NONE},
    [TOKEN_RIGHT_BRACE]     = {NULL,        NULL,       PREC_NONE},
    [TOKEN_LEFT_BRACKET]    = {NULL,        NULL,       PREC_NONE},
    [TOKEN_RIGHT_BRACKET]   = {NULL,        NULL,       PREC_NONE},
    [TOKEN_COMMA]           = {NULL,        NULL,       PREC_NONE},
    [TOKEN_DOT]             = {NULL,        NULL,       PREC_NONE},
    [TOKEN_MINUS]           = {unary,       binary,     PREC_TERM},
    [TOKEN_PLUS]            = {NULL,        binary,     PREC_TERM},
    [TOKEN_SEMICOLON]       = {NULL,        NULL,       PREC_NONE},
    [TOKEN_SLASH]           = {NULL,        binary,     PREC_FACTOR},
    [TOKEN_STAR]            = {NULL,        binary,     PREC_FACTOR},
    [TOKEN_BANG]            = {NULL,        NULL,       PREC_NONE},
    [TOKEN_BANG_BANG]       = {NULL,        NULL,       PREC_NONE},
    [TOKEN_BANG_EQUAL]      = {NULL,        NULL,       PREC_NONE},
    [TOKEN_EQUAL]           = {NULL,        NULL,       PREC_NONE},
    [TOKEN_EQUAL_EQUAL]     = {NULL,        NULL,       PREC_NONE},
    [TOKEN_GREATER]         = {NULL,        NULL,       PREC_NONE},
    [TOKEN_GREATER_EQUAL]   = {NULL,        NULL,       PREC_NONE},
    [TOKEN_LESSER]          = {NULL,        NULL,       PREC_NONE},
    [TOKEN_LESSER_EQUAL]    = {NULL,        NULL,       PREC_NONE},
    [TOKEN_IDENTIFIER]      = {NULL,        NULL,       PREC_NONE},
    [TOKEN_STRING]          = {NULL,        NULL,       PREC_NONE},
    [TOKEN_NUMBER]          = {number,      NULL,       PREC_NONE},
    [TOKEN_AND]             = {NULL,        NULL,       PREC_NONE},
    [TOKEN_CLASS]           = {NULL,        NULL,       PREC_NONE},
    [TOKEN_ELSE]            = {NULL,        NULL,       PREC_NONE},
    [TOKEN_FALSE]           = {NULL,        NULL,       PREC_NONE},
    [TOKEN_FOR]             = {NULL,        NULL,       PREC_NONE},
    [TOKEN_FUN]             = {NULL,        NULL,       PREC_NONE},
    [TOKEN_IF]              = {NULL,        NULL,       PREC_NONE},
    [TOKEN_NIL]             = {NULL,        NULL,       PREC_NONE},
    [TOKEN_OR]              = {NULL,        NULL,       PREC_NONE},
    [TOKEN_PRINT]           = {NULL,        NULL,       PREC_NONE},
    [TOKEN_RETURN]          = {NULL,        NULL,       PREC_NONE},
    [TOKEN_THIS]            = {NULL,        NULL,       PREC_NONE},
    [TOKEN_SUPER]           = {NULL,        NULL,       PREC_NONE},
    [TOKEN_TRUE]            = {NULL,        NULL,       PREC_NONE},
    [TOKEN_VAR]             = {NULL,        NULL,       PREC_NONE},
    [TOKEN_WHILE]           = {NULL,        NULL,       PREC_NONE},
    [TOKEN_ERROR]           = {NULL,        NULL,       PREC_NONE},
    [TOKEN_EOF]             = {NULL,        NULL,       PREC_NONE},
};

// PRECEDENCE is a NUMERIC value that says when you should stop parsing.
// You should ONLY parse expressions HIGHER than your precedence.
//THIS DOES ALL THE HARD WORK
static void parsePrecedence(Precedence precedence){
    //Advance to the next token, knowing our current precedence level
    advance();
    //Get the prefix rule from the above table.
    ParseFn prefixRule = getRule(parser.previous.type)->prefix;
    //Do we have a prefix for this token?
    if(prefixRule == NULL){
        error("Expect expression");
        return;
    }
    //Execute the prefix rule.
    prefixRule();

    //Read the following tokens, then execute any infix rules.
    while(precedence <= getRule(parser.current.type)->precedence){
        advance();
        ParseFn infixRule = getRule(parser.previous.type)->infix;
        infixRule();
    }
}

static ParseRule* getRule(TokenType type){
    return &rules[type];
}



// Compile

bool compile(const char* source, Chunk* chunk){
    //Prime the scanner by feeding it the source.
    initScanner(source);
    compilingChunk = chunk;
    advance();
    expression();
    consume(TOKEN_EOF, "Expect end of expression");
    endCompiler();
    return !parser.hadError;
}

    // Old troubleshoot compiler
    // //Prime the scanner by feeding it the source.
    // initScanner(source);
    // //Internal line count for the compiler;
    // int line = -1;
    // for (;;){
    //     //Read a token
    //     Token token = scanToken();
    //     if(token.line != line){
    //         printf("%4d ", token.line);
    //         line = token.line;
    //     } else {
    //         printf("   |");
    //     }
    //     printf("%2d \'%.*s\'\n", token.type, token.length, token.start);
    //     if (token.type == TOKEN_EOF) break;
    // }