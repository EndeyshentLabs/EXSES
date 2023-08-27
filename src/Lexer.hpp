#ifndef EXSES_LEXER_H
#define EXSES_LEXER_H

#include <map>
#include <string>
#include <vector>

#include <Position.hpp>
#include <Procedure.hpp>
#include <Token.hpp>

enum Target {
    EXSI
};

class Lexer {
public:
    Lexer(std::string fileName, Target target);
    Target target;
    void lexSource();
    void intrepret(bool insideOfProc = false, std::vector<Token> procBody = {});
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

    std::map<std::string, Value> storage;
    std::vector<Procedure> procedureStorage;

    TokenType makeType(std::string text);
    void makeError(Token token, std::string text);
    std::string tokenLocation(Token token);
    void processStringLiteral(Token& token);
    void processToken(Token& token, bool inside);
    bool processFolded(Token& token, TokenType startType, TokenType endType, std::vector<Token>& body);
    void createToken(std::string text, unsigned int col);
};

template <typename T> // This trick is needed for compatibility with both Token and Procedure classes
void printTokenLineInfo(T token);

#endif /* EXSES_LEXER_H */
