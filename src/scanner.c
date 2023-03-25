#include <stdio.h>
#include <string.h>

#include "scanner.h"
#include "Bnuuy_common.h"

Scanner scanner;

void initScanner(const char* source){
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

static char advance() {
    scanner.current++;
    return scanner.current[-1];
}

static char peek() {
    return *scanner.current;
}

static bool isAtEnd(){
    return *scanner.current == '\0';
}


static char peekNext() {
    if(isAtEnd()) return '\0';
    return *(scanner.current + 1);
}

static bool match(char expected){
    if(isAtEnd()) return false;
    if (*scanner.current != expected) return false;
    scanner.current++;
    return true;
}

static bool isDigit(char c){
    return '0' <= c && c <= '9';
}

static bool isAlpha(char c){
    return ('a' <= c && c <= 'z' ) || ('A' <= c && c <= 'Z') || (c == '_');
}


static Token makeToken(TokenType type){
    Token token;
    token.type = type;
    token.start = scanner.start;
    token.length = (int) (scanner.current - scanner.start);
    token.line = scanner.line;
    return token;
}

static Token errorToken(const char* message){
    Token token;
    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = (int) strlen(message);
    token.line = scanner.line;
    return token;
}

static void skipWhitespace(){
    for (;;) {
        char c = peek();
        switch(c){
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                scanner.line++;
                advance();
                break;
            case '#':
                //Comments in my script will be # like python
                while( (peek() != '\n' && !isAtEnd())) advance();
            default:
                return;
        }
    }
}

static TokenType checkWord(int start, int length, const char* remainder, TokenType type){
    if((scanner.current - scanner.start == start + length) && strcmp(scanner.start + start, remainder + length)) {
        return type;
    }
    return TOKEN_IDENTIFIER;
}

static Token string(){
    while (peek() != '"' && !isAtEnd()){
        if( peek() == '\n') scanner.line++;
        advance();
    }

    if(isAtEnd()) return errorToken("Unterminated string");
    //The character after peek() is the end quote to break out of the top while.
    advance();
    return makeToken(TOKEN_STRING);
}

static Token number() {
    while( isDigit(peek())) advance();

    //Consider deicmal numbers.
    if (peek() == '.' && isDigit(peekNext())) advance(); // Eat the decimal

    while (isDigit(peek())) advance();

    return makeToken(TOKEN_NUMBER);
}

//DFA - Deterministic finite algorithm/Automation / Finite-state machine
static TokenType identifierType() {
    switch(scanner.start[0] ){
        case 'a': return checkWord(1, 2, "nd", TOKEN_AND);
        case 'c': return checkWord(1, 4, "lass", TOKEN_CLASS);
        case 'e': return checkWord(1, 3, "lse", TOKEN_ELSE);
        case 'i': return checkWord(1, 1, "f", TOKEN_IF);
        case 'n': return checkWord(1, 2, "il", TOKEN_NIL);
        case 'o': return checkWord(1, 1, "r", TOKEN_OR);
        case 'p': return checkWord(1, 4, "rint", TOKEN_PRINT);
        case 'r': return checkWord(1, 5, "eturn", TOKEN_RETURN);
        case 't': 
            //'t' is also allowed so we should see if there is anything left in this buffer
            if(scanner.current - scanner.start > 1){
                switch(scanner.start[1]){
                    case 'h': return checkWord(2, 2, "is", TOKEN_THIS);
                    case 'r': return checkWord(2, 2, "ue", TOKEN_TRUE);
                }
            }
        case 'f':
            if(scanner.current - scanner.start > 1){
                switch(scanner.start[1]){
                    case 'o': return checkWord(2, 1, "r", TOKEN_FOR);
                    case 'a': return checkWord(2, 3, "lse", TOKEN_FALSE);
                    case 'u': return checkWord(2, 1, "n", TOKEN_FUN);
                }
            }
        case 's': return checkWord(1, 3, "uper", TOKEN_SUPER);
        case 'v': return checkWord(1, 2, "ar", TOKEN_VAR);
        case 'w': return checkWord(1, 4, "hile", TOKEN_WHILE);
    }
    return TOKEN_IDENTIFIER;
}

static Token identifier(){
    //Descent switch growing pattern matcher.
    //While we have more characters to match
    while( isAlpha(peek()) || isDigit(peek())) advance();
    //If we hit a whitespace we return
    return makeToken(identifierType());
}

Token scanToken(){
    //Skip all whitespace at the start of this token.
    skipWhitespace();
    //Advance the pointer of the start of this token to the current index
    scanner.start = scanner.current;
    //Write EOF if eof
    if(isAtEnd()) return makeToken(TOKEN_EOF);

    //Advance the character pointer;
    char c = advance();
    //If the symbol begins with an alphabetical character, match it for identifiers/reserved keywords.
    if( isAlpha(c)) return identifier();
    //If the symbol immediately starts with a number, it is only valid to be a number in clox
    if( isDigit(c)) return number();

    switch(c){
        //Single characters
        case '(': return makeToken(TOKEN_LEFT_PAREN);
        case ')': return makeToken(TOKEN_RIGHT_PAREN);
        case '[': return makeToken(TOKEN_LEFT_BRACKET);
        case ']': return makeToken(TOKEN_RIGHT_BRACKET);
        case '{': return makeToken(TOKEN_LEFT_BRACE);
        case '}': return makeToken(TOKEN_RIGHT_BRACE);
        case ';': return makeToken(TOKEN_SEMICOLON);
        case ',': return makeToken(TOKEN_COMMA);
        case '.': return makeToken(TOKEN_DOT);
        case '-': return makeToken(TOKEN_MINUS);
        case '+': return makeToken(TOKEN_PLUS);
        case '/': return makeToken(TOKEN_SLASH);
        case '*': return makeToken(TOKEN_STAR);
        //Doubles
        case '!': return makeToken( match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG); 
        case '=': return makeToken( match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        case '<': return makeToken( match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        case '>': return makeToken( match('=') ? TOKEN_LESSER_EQUAL : TOKEN_LESSER);
        //String literal
        case '"': return string();
    }

    return errorToken("Unexpected character.");
}