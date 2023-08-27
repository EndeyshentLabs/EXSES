#include <Position.hpp>

#include <format>

std::string Position::toString()
{
    return std::format("%u:%u", this->line, this->col);
}
