#include <Token.hpp>

#include <Location.hpp>

Token::Token(TokenType type, int value) : type(type), value(value)
{
}

Token::Token(TokenType type) : type(type)
{
}

Token::Token(TokenType type, int value, Location loc) : type(type), loc(loc), value(value)
{
}

Token::Token(TokenType type, Location loc) : type(type), loc(loc)
{
}
