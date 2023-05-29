#ifndef EXSES_TOKEN_H
#define EXSES_TOKEN_H

#include <string>

enum TokenType
{
    UNDEFINED = -1,
    PUSH = 0,
    DUP,
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
    TERNARY, // NOTE: Maybe IFX?
    MAKEPROC,
    ENDPROC,
    INVOKEPROC
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
