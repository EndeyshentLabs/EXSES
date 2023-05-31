#ifndef EXSES_TOKEN_H
#define EXSES_TOKEN_H

#include <string>

enum TokenType
{
    UNDEFINED = -1,
    PUSH = 0,
    DUP,
    OVER,
    DROP,
    SWAP,
    PLUS,
    MINUS,
    MULT,
    DIV,
    DUMP,
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
    STDLIB_MATH_SQRT,
    STDLIB_MATH_COS,
    STDLIB_MATH_SIN
};

extern std::string TokenTypeString[];

struct Token
{
    unsigned int line;
    unsigned int col;
    TokenType type;
    std::string text;
    bool enabled = true;
};

#endif /* EXSES_TOKEN_H */
