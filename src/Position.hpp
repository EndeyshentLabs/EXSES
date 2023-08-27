#ifndef EXSES_POSITION_H
#define EXSES_POSITION_H

#include <string>

class Position {
public:
    Position(const Position&) = default;
    Position(Position&&) = default;
    Position& operator=(const Position&) = default;
    Position& operator=(Position&&) = default;
    Position(unsigned int line, unsigned int col)
        : line(line)
        , col(col)
    {
    }
    Position() = default;

    unsigned int line;
    unsigned int col;

    std::string toString();
};

#endif /* EXSES_POSITION_H */
