#ifndef XESVM_INSTRUCTION_H
#define XESVM_INSTRUCTION_H

#include <Word.hpp>
#include <string>

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
    PRINT,
    RET
};

static std::string instructionTypeToString(InstructionType type) {
    switch (type) {
        case InstructionType::NOP: return "NOP"; break;
        case InstructionType::PUSH: return "PUSH"; break;
        case InstructionType::DUP: return "DUP"; break;
        case InstructionType::PLUS: return "PLUS"; break;
        case InstructionType::MINUS: return "MINUS"; break;
        case InstructionType::MULT: return "MULT"; break;
        case InstructionType::DIV: return "DIV"; break;
        case InstructionType::JMP: return "JMP"; break;
        case InstructionType::JNZ: return "JNZ"; break;
        case InstructionType::EQUALS: return "EQUALS"; break;
        case InstructionType::HALT: return "HALT"; break;
        case InstructionType::PRINT: return "PRINT"; break;
        case InstructionType::RET: return "RET"; break;
    }
}

struct Instruction {
    InstructionType type;
    Word operand;
};

#endif // XESVM_INSTRUCTION_H
