#ifndef EXSES_LEXER_H
#define EXSES_LEXER_H

#include <map>
#include <string>
#include <vector>

#include <Token.hpp>
#include <Procedure.hpp>

enum Target {
    EXSI
};

class Lexer
{
public:
    Lexer(std::string fileName, Target target);
    Target target;
    void tokenize();
    void intrepret(bool insideOfProc = false, std::vector<Token> procBody = {});
    void compileToPython3();
    void run();
private:
    std::string source;
    std::string fileName;
    std::vector<Token> program;
    std::vector<int> stack;
    std::map<int, int> storage;
    std::vector<Procedure> procedureStorage;
    void makeError(Token token, std::string text);
    std::string tokenLocation(Token token);
};

#endif /* EXSES_LEXER_H */
