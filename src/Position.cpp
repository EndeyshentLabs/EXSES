#include <Position.hpp>

#include <fmt/format.h>

std::string Position::toString()
{
    return fmt::format("{}:{}:{}", this->fileName, this->line + 1, this->col);
}
