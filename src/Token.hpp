#ifndef EXSES_TOKEN_H
#define EXSES_TOKEN_H

#include <string>

#include <Position.hpp>
#include <Value.hpp>
#include <vector>

enum TokenType {
    UNDEFINED = -1,
    PUSH = 0,
    STRING,
    STRING_DUMP,
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

    IDENT,
};

static const std::string TokenTypeString[] = {
    "PUSH",
    "STRING",
    "STRING_DUMP",
    "STRING_PLUS",
    "DUP",
    "OVER",
    "DROP",
    "SWAP",
    "PLUS",
    "MINUS",
    "MULT",
    "DIV",
    "DUMP",
    "INPUT",
    "BIND",
    "SAVE",
    "LOAD",
    "TERNARY",
    "MAKEPROC",
    "ENDPROC",
    "INVOKEPROC",
    "IF",
    "ENDIF",
    "EQUAL",
    "NOTEQUAL",
    "LESS",
    "LESSEQUAL",
    "GREATER",
    "GREATEREQUAL",
    "LOR",
    "LAND",
    "LNOT",
    "TRUE",
    "FALSE",

    "IDENT",
};

static const std::vector<std::string> Keywords = {
    "s!",
    "s+",
    "&",
    "$&",
    "_",
    "$",
    "+",
    "-",
    "*",
    "/",
    "!",
    "@",
    "<-",
    "<!",
    "^",
    "?:",
    "'",
    "\"",
    ":",
    "(",
    ")",
    "=",
    "<>",
    "<",
    "<=",
    ">",
    ">=",
    "||",
    "&&",
    "!!",
    "true",
    "false",
};

TokenType tokenTypeFromString(std::string str);

struct Token {
    Token(Position pos, TokenType type, std::string text, Value value)
        : pos(pos)
        , type(type)
        , text(std::move(text))
        , value(value)
    {
    }
    Token(const Token&) = default;
    Token(Token&&) = default;
    Token& operator=(const Token&) = delete;
    Token& operator=(Token&&) = delete;
    Position pos;
    TokenType type;
    const std::string text;
    Value value;
    // For IDENTs
    bool processedByParser;
    // Only for blocks
    unsigned int pairIp = 0;
};

#endif /* EXSES_TOKEN_H */
