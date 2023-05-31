#ifndef XESVM_INSTRUCTION_H
#define XESVM_INSTRUCTION_H

#include <Word.hpp>

enum class InstructionType {
    NOP = 0,
    PUSH,
    DUP,
    PLUS,
    MINUS,
    MULT,
    DIV,
    JMP,
    JNZ,
    EQUALS,
    HALT,
    PRINT
};

struct Instruction {
    InstructionType type;
    Word operand;
};
#endif // XESVM_INSTRUCTION_H
