#ifndef EXSES_PARSER_H
#define EXSES_PARSER_H

#include <vector>

#include <Token.hpp>
#include <Lexer.hpp>

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
    unsigned int ip;
    std::vector<Token> program;

    void compileToNasmLinux86_64();
};

#endif /* EXSES_PARSER_H */