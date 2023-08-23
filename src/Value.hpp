#ifndef EXSES_VALUE_H
#define EXSES_VALUE_H

#include <string>

enum class ValueType {
    INT,
    STRING
};

struct Value {
    Value(const Value&) = default;
    Value(Value&&) = default;
    Value& operator=(const Value&) = default;
    Value& operator=(Value&&) = default;
    Value(ValueType type, std::string text)
        : type(type)
        , text(std::move(text))
    {
    }
    ValueType type;
    std::string text;
    bool operator==(Value& other);
    bool operator!=(Value& other);
};

#endif /* EXSES_VALUE_H */
