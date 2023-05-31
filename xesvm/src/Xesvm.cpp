#include <Xesvm.hpp>
#include <iostream>

Xesvm gVm;

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

            Word b = this->stack.back(); this->stack.pop_back();
            Word a = this->stack.back(); this->stack.pop_back();
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

            Word b = this->stack.back(); this->stack.pop_back();
            Word a = this->stack.back(); this->stack.pop_back();

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
            ++this->ip;
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
