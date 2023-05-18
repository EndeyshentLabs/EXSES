#ifndef EXSES_LOCATION_H
#define EXSES_LOCATION_H

#include <string>
class Location
{
public:
    Location() = default;
    Location(std::string fileName, int row, int col);
    std::string fileName;
    int row = 0;
    int col = 0;
};

#endif /* EXSES_LOCATION_H */
