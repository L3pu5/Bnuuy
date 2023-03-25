#ifndef SCANNER
#define SCANNER

typedef enum {
    //Single characters
    TOKEN_LEFT_PAREN,           TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACE,           TOKEN_RIGHT_BRACE,
    TOKEN_LEFT_BRACKET,         TOKEN_RIGHT_BRACKET,
    TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,
    TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR,
    //1 OR 2 characters
    TOKEN_BANG,                 TOKEN_BANG_BANG, TOKEN_BANG_EQUAL,
    TOKEN_EQUAL,                TOKEN_EQUAL_EQUAL,
    TOKEN_GREATER,              TOKEN_GREATER_EQUAL,
    TOKEN_LESSER,               TOKEN_LESSER_EQUAL,
    //Literals
    TOKEN_IDENTIFIER,
    TOKEN_STRING,
    TOKEN_NUMBER,
    //Reserverd Keywords
    //Organising these alphabetically makes life easier
    TOKEN_AND, TOKEN_CLASS, TOKEN_ELSE, TOKEN_FALSE, TOKEN_FOR, TOKEN_FUN, TOKEN_IF, TOKEN_NIL, TOKEN_OR, TOKEN_PRINT, TOKEN_RETURN, TOKEN_THIS, TOKEN_SUPER, TOKEN_TRUE, TOKEN_VAR, TOKEN_WHILE,

    TOKEN_ERROR, TOKEN_EOF 
} TokenType;

typedef struct {
    const char* start;
    const char* current;
    int line;
} Scanner;

typedef struct {
    TokenType type;
    const char* start;
    int length;
    int line;
} Token;


void initScanner(const char* source);
Token scanToken();

#endif 