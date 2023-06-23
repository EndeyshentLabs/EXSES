#ifndef EXSES_PROCEDURE_H
#define EXSES_PROCEDURE_H

#include <vector>

#include <Token.hpp>

class Procedure
{
public:
    Procedure(int line, int col, std::string name, std::vector<Token> body);
    unsigned int line;
    unsigned int col;
    std::string name;
    std::vector<Token> getBody() { return body; }
private:
    std::vector<Token> body;
};

#endif /* EXSES_PROCEDURE_H */
