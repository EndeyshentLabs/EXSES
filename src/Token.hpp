#ifndef EXSES_TOKEN_H
#define EXSES_TOKEN_H

#include <string>

enum TokenType
{
    UNDEFINED = -1,
    PUSH = 0,
    DUP,
    SWAP,
    PLUS,
    MINUS,
    MULT,
    DIV,
    DUMP,
    BIND,
    SAVE,
    LOAD
};

struct Token
{
    int line;
    int col;
    TokenType type;
    std::string text;
};

#endif /* EXSES_TOKEN_H */
