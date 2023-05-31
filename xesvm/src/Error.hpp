#ifndef XESVM_ERROR_H
#define XESVM_ERROR_H
#include <string>

enum class Error {
    OK = 0,
    STACK_UNDERFLOW,
    ILLEGAL_ACCESS,
    ILLEGAL_OPERAND,
    ILLEGAL_INSTRUCTION,
    DIVISION_BY_ZERO
};

static std::string errorToString(Error err) {
    switch (err) {
        case Error::OK: return "OK"; break;
        case Error::STACK_UNDERFLOW: return "STACK_UNDERFLOW"; break;
        case Error::ILLEGAL_ACCESS: return "ILLEGAL_ACCESS"; break;
        case Error::ILLEGAL_OPERAND: return "ILLEGAL_OPERAND"; break;
        case Error::ILLEGAL_INSTRUCTION: return "ILLEGAL_INSTRUCTION"; break;
        case Error::DIVISION_BY_ZERO: return "DIVISION_BY_ZERO"; break;
    }
}

static std::string ErrorString[]= {
    "OK",
    "STACK_UNDERFLOW",
    "ILLEGAL_ACCESS",
    "ILLEGAL_OPERAND",
    "ILLEGAL_INSTRUCTION"
};
#endif // XESVM_ERROR_H
