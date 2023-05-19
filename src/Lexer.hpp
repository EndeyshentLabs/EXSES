#ifndef EXSES_LEXER_H
#define EXSES_LEXER_H

#include <map>
#include <string>
#include <vector>

#include <Token.hpp>

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
    void intrepret();
    void compileToPython3();
    void run();
private:
    std::string source;
    std::string fileName;
    std::vector<Token> program;
    std::vector<int> stack;
    std::map<int, int> storage;
    void makeError(Token token, std::string text);
};

#endif /* EXSES_LEXER_H */
