#ifndef EXSES_LEXER_H
#define EXSES_LEXER_H

#include <map>
#include <string>
#include <vector>

#include <Position.hpp>
#include <Token.hpp>

enum Target {
    EXSI,
    NASM_LINUX_X86_64,
    NASM_WIN32, // NOTE: Not supported
#ifdef SUPPORT_LLVM
    LLVM, // NOTE: Not supported
#endif
};

class Lexer {
public:
    Lexer(std::string fileName, Target target);
    Target target;
    void lexSource();
    void intrepret();
    void run();

private:
    std::string source;
    std::string fileName;
    std::vector<Token> program;
    std::vector<Value> stack;
    char curChar = source[0];
    Position pos;
    unsigned int cursor;

    void advance();

    Token makeNumber();
    Token makeString();

    void linkBlocks();
};

template <typename T> // This trick is needed for compatibility with both Token and Procedure classes
void printTokenLineInfo(T token);

#endif /* EXSES_LEXER_H */
