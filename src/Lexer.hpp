#ifndef EXSES_LEXER_H
#define EXSES_LEXER_H

#include <string>
#include <vector>

#include <Token.hpp>
#include <Location.hpp>

enum Target {
    PYTHON3,
    EXSI
};

class Lexer
{
public:
    Lexer(std::string fileName, std::string source, Target target);
    Target target;
    void tokenize();
    void parse();
    void intrepret();
private:
    std::string source;
    std::string fileName;
    std::vector<Token> program;
    std::vector<int> stack;
    int row = 0;
    int bol = 0;
    int cur = 0;
    bool isNotEmpty() { return this->cur < source.length(); }
    bool isEmpty() { return !isNotEmpty(); }
    void nextToken();
    void chopChar();
    void trimLeft();
    void dropLine();
    Location loc();
    Token makeNumber();
};

#endif /* EXSES_LEXER_H */
