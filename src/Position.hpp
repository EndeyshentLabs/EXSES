#ifndef EXSES_POSITION_H
#define EXSES_POSITION_H

#include <string>

class Position {
public:
    Position(const Position&) = default;
    Position(Position&&) = default;
    Position& operator=(const Position&) = default;
    Position& operator=(Position&&) = default;
    Position(std::string fileName, unsigned int line, unsigned int col)
        : fileName(std::move(fileName))
        , line(line)
        , col(col)
    {
    }
    Position() = default;

    std::string fileName;
    unsigned int line = 0;
    unsigned int col = 1;

    std::string toString();
};

#endif /* EXSES_POSITION_H */
