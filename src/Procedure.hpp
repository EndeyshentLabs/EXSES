#ifndef EXSES_PROCEDURE_H
#define EXSES_PROCEDURE_H

#include <vector>

#include <Token.hpp>

class Procedure
{
public:
    Procedure(int line, int col, int name, std::vector<Token> body); // : line(line), col(col), name(name), body(body) {}
    unsigned int line;
    unsigned int col;
    int name;
    std::vector<Token> getBody() { return body; }
private:
    std::vector<Token> body;
};

#endif /* EXSES_PROCEDURE_H */
