#include <Position.hpp>

#include <format>

std::string Position::toString()
{
    return std::format("{}:{}", this->line + 1, this->col + 1);
}
