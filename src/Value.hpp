#ifndef EXSES_VALUE_H
#define EXSES_VALUE_H

#include <string>

enum class ValueType {
    INT,
    STRING
};

struct Value {
    ValueType type;
    std::string text;
    bool operator==(Value& other);
    bool operator!=(Value& other);
};

#endif /* EXSES_VALUE_H */
