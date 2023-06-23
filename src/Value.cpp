#include <Value.hpp>

bool Value::operator==(Value& other)
{
    return this->type == other.type && this->text == other.text;
}

bool Value::operator!=(Value& other)
{
    return this->type != other.type && this->text != other.text;
}
