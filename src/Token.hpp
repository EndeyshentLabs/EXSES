#ifndef EXSES_TOKEN_H
#define EXSES_TOKEN_H

#include <string>

#include <Value.hpp>

enum TokenType {
    UNDEFINED = -1,
    PUSH = 0,
    STRING,
    STRING_PLUS,
    DUP,
    OVER,
    DROP,
    SWAP,
    PLUS,
    MINUS,
    MULT,
    DIV,
    DUMP,
    INPUT,
    BIND,
    SAVE,
    LOAD,
    TERNARY,
    MAKEPROC,
    ENDPROC,
    INVOKEPROC,
    IF,
    ENDIF,
    EQUAL,
    NOTEQUAL,
    LESS,
    LESSEQUAL,
    GREATER,
    GREATEREQUAL,
    LOR, // Logical OR (aka ||)
    LAND, // Logical AND (aka &&)
    LNOT, // Logical NOT (aka !)
    TRUE,
    FALSE,
};

extern std::string TokenTypeString[];

struct Token {
    unsigned int line;
    unsigned int col;
    TokenType type;
    const std::string text;
    Value value;
    bool enabled = true;
};

#endif /* EXSES_TOKEN_H */
