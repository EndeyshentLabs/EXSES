#include <cstdint>
#include <cstddef>
#include <iostream>
#include <vector>

// !NOTE! This is just a concept !NOTE! //

// TODO: Separate this file into multiple files
// TODO: Assembly language for XESVM (xesasm?)
// TODO: XESVM bytecode

#define EXECUTION_LIMIT 8192 // TODO: tweak this value or remove

using Word = std::int64_t;

enum class Error {
    OK = 0,
    STACK_UNDERFLOW,
    ILLEGAL_ACCESS,
    ILLEGAL_OPERAND,
    ILLEGAL_INSTRUCTION,
    DIVISION_BY_ZERO
};

std::string errorToString(Error err) {
    switch (err) {
        case Error::OK: return "OK"; break;
        case Error::STACK_UNDERFLOW: return "STACK_UNDERFLOW"; break;
        case Error::ILLEGAL_ACCESS: return "ILLEGAL_ACCESS"; break;
        case Error::ILLEGAL_OPERAND: return "ILLEGAL_OPERAND"; break;
        case Error::ILLEGAL_INSTRUCTION: return "ILLEGAL_INSTRUCTION"; break;
        case Error::DIVISION_BY_ZERO: return "DIVISION_BY_ZERO"; break;
    }
}

std::string ErrorString[]= {
    "OK",
    "STACK_UNDERFLOW",
    "ILLEGAL_ACCESS",
    "ILLEGAL_OPERAND",
    "ILLEGAL_INSTRUCTION"
};

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

class Xesvm {
public:
    std::vector<Word> stack;
    std::vector<Instruction> program;
    Word ip = 0;
    bool halt = false;

    Error executeInstruction();
};

Error Xesvm::executeInstruction() {
    if (this->ip < 0 || this->ip >= this->program.size()) {
        return Error::ILLEGAL_ACCESS;
    }

    Instruction instruction = this->program[this->ip];

    switch (instruction.type) {
        case InstructionType::NOP: {
            ++this->ip;
        } break;
        case InstructionType::PUSH: {
            this->stack.push_back(instruction.operand);

            ++this->ip;
        } break;
        case InstructionType::PLUS: {
            if (this->stack.size() < 2) {
                return Error::STACK_UNDERFLOW;
            }

            Word a = this->stack.back(); this->stack.pop_back();
            Word b = this->stack.back(); this->stack.pop_back();
            this->stack.push_back(a + b);

            ++this->ip;
        } break;
        case InstructionType::MINUS: {
            if (this->stack.size() < 2) {
                return Error::STACK_UNDERFLOW;
            }

            Word a = this->stack.back(); this->stack.pop_back();
            Word b = this->stack.back(); this->stack.pop_back();
            this->stack.push_back(a - b);

            ++this->ip;
        } break;
        case InstructionType::MULT: {
            if (this->stack.size() < 2) {
                return Error::STACK_UNDERFLOW;
            }

            Word a = this->stack.back(); this->stack.pop_back();
            Word b = this->stack.back(); this->stack.pop_back();
            this->stack.push_back(a * b);

            ++this->ip;
        } break;
        case InstructionType::DIV: {
            if (this->stack.size() < 2) {
                return Error::STACK_UNDERFLOW;
            }

            Word a = this->stack.back(); this->stack.pop_back();
            Word b = this->stack.back(); this->stack.pop_back();

            if (b == 0) {
                return Error::DIVISION_BY_ZERO;
            }

            this->stack.push_back(a / b);

            ++this->ip;
        } break;
        case InstructionType::JMP: {
            this->ip = instruction.operand;
        } break;
        case InstructionType::JNZ: {
            if (this->stack.size() < 1) {
                return Error::STACK_UNDERFLOW;
            }

            Word a = this->stack.back(); this->stack.pop_back();

            if (a != 0) {
                this->ip = instruction.operand;
            } else {
                ++this->ip;
            }
        } break;
        case InstructionType::PRINT: {
            if (this->stack.size() < 1) {
                return Error::STACK_UNDERFLOW;
            }

            Word a = this->stack.back(); this->stack.pop_back();

            std::cout << a << '\n';
        } break;
        case InstructionType::DUP: {
            if (this->stack.size() < 1) {
                return Error::STACK_UNDERFLOW;
            }
            this->stack.push_back(this->stack[this->stack.size() - 1 - instruction.operand]);
        } break;
        case InstructionType::EQUALS: {
            if (this->stack.size() < 2) {
                return Error::STACK_UNDERFLOW;
            }

            Word a = this->stack.back(); this->stack.pop_back();
            Word b = this->stack.back(); this->stack.pop_back();
            this->stack.push_back(a == b);

            ++this->ip;
        } break;
        case InstructionType::HALT: {
            this->halt = true;
        } break;
        default: { return Error::ILLEGAL_INSTRUCTION; } break;
    }

    return Error::OK;
}

int main()
{
    Xesvm vm;
    vm.program = {                                                   // addr
        Instruction { .type = InstructionType::PUSH, .operand = 34}, // 0
        Instruction { .type = InstructionType::PUSH, .operand = 35}, // 1
        Instruction { .type = InstructionType::PLUS },               // 2
        Instruction { .type = InstructionType::JMP, .operand = 1 },  // 3
    };

    for (unsigned int i = 0; i < EXECUTION_LIMIT && !vm.halt; i++) {
        Error error = vm.executeInstruction();

        std::cout << "STACK:\n";
        for (Word j : vm.stack) {
            std::cout << "\t" << j << '\n';
        }

        if (error != Error::OK) {
            std::cerr << "ERROR: " << errorToString(error) << '\n';
            std::exit(1);
        }
    }

    std::cout << "Execution ended\n";

    return 0;
}
