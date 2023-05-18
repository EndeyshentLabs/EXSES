#ifndef EXSES_TOKEN_H
#define EXSES_TOKEN_H

#include "Location.hpp"
enum TokenType
{
    PUSH,
    DUP,
    SWAP,
    PLUS,
    MINUS,
    MULT,
    DIV,
    DUMP
};

class Token
{
public:
    Token(TokenType type, int value);
    Token(TokenType type, int value, Location loc);
    Token(TokenType type);
    Token(TokenType type, Location loc);
    TokenType type;
    Location loc;
    int getValue() { return this->value; };
private:
    int value; // TODO: Add different types
};

#endif /* EXSES_TOKEN_H */
