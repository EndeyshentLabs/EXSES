#ifndef EXSES_PARSER_H
#define EXSES_PARSER_H

#include <vector>

#include <Lexer.hpp>
#include <Token.hpp>

typedef unsigned int InstructionAddr;

enum class BindingSize {
    BYTE = 8,
    WORD = 16,
    DWORD = 32,
    QWORD = 64
};

class Parser {
public:
    Parser(Target target, std::string inputFileName, std::vector<Token> program)
        : target(target)
        , inputFileName(std::move(inputFileName))
        , program(std::move(program))
    {
        this->parse();
    }
    Parser(const Parser&) = default;
    Parser(Parser&&) = default;
    Parser& operator=(const Parser&) = default;
    Parser& operator=(Parser&&) = default;
    Target target;

    void parse();

private:
    std::string inputFileName;
    InstructionAddr ip = 0;
    std::vector<Token> program;
    bool hadError = false;

    void compileToNasmLinux86_64();
    void intrepret();

    Value evalConstExpr(InstructionAddr start, InstructionAddr end);

    void error(Token token, std::string msg);
    void note(Token token, std::string msg);
};

#endif /* EXSES_PARSER_H */
