#include <Procedure.hpp>

Procedure::Procedure(int line, int col, std::string name, std::vector<Token> body)
    : line(line)
    , col(col)
    , name(name)
    , body(body)
{
}
