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
    LOAD8,
    LOAD16,
    LOAD32,
    LOAD64,
    SIZE,
    TERNARY,
    MAKEPROC,
    ENDPROC,
    INVOKEPROC,
    IF,
    ENDIF,
    WHILE,
    DOWHILE,
    ENDWHILE,
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
    // Register manipulation
    TORAX,
    TORBX,
    TORCX,
    TORDX,
    TORSI,
    TORDI,
    TORBP,
    TOR8,
    TOR9,
    TOR10,
    SYSTEM_SYSCALL,

    // Identifier
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
    "LOAD8",
    "LOAD16",
    "LOAD32",
    "LOAD64",
    "SIZE",
    "TERNARY",
    "MAKEPROC",
    "ENDPROC",
    "INVOKEPROC",
    "IF",
    "ENDIF",
    "WHILE",
    "DOWHILE",
    "ENDWHILE",
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
    "TORAX",
    "TORBX",
    "TORCX",
    "TORDX",
    "TORSI",
    "TORDI",
    "TORBP",
    "TOR8",
    "TOR9",
    "TOR10",
    "SYSTEM_SYSCALL",

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
    "^8",
    "^16",
    "^32",
    "^64",
    "byte",
    "word",
    "dword",
    "qword",
    ".?",
    "'",
    "\"",
    ":",
    "(",
    ")",
    "{",
    "=>",
    "}",
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
    ">rax",
    ">rbx",
    ">rcx",
    ">rdx",
    ">rsi",
    ">rdi",
    ">rbp",
    ">r8",
    ">r9",
    ">r10",
    "\\syscall",
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
    // Only for blocks
    unsigned int pairIp = 0;
};

#endif /* EXSES_TOKEN_H */
