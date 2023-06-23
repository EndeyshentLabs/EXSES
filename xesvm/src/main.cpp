#include <iostream>
#include <vector>

#define EXECUTION_LIMIT 64 // TODO: tweak this value or remove

#include <Instruction.hpp>
#include <Error.hpp>
#include <Xesvm.hpp>

// !NOTE! This is just a concept !NOTE! //

// TODO: Assembly language for XESVM (xesasm?)
// TODO: XESVM bytecode

int main()
{
    // gVm.program = {                                                  // addr
    //     Instruction { .type = InstructionType::PUSH, .operand = 0 }, // 0
    //     Instruction { .type = InstructionType::PUSH, .operand = 1 }, // 1
    //     Instruction { .type = InstructionType::DUP, .operand = 1 },  // 2
    //     Instruction { .type = InstructionType::DUP, .operand = 1 },  // 3
    //     Instruction { .type = InstructionType::PLUS },               // 4
    //     Instruction { .type = InstructionType::JMP, .operand = 2 }   // 5
    // };

    gVm.program = {
        Instruction{ .type = InstructionType::PUSH, .operand = 99 }, // 0
        Instruction{ .type = InstructionType::PUSH, .operand = 86 }, // 1
        Instruction{ .type = InstructionType::JMP, .operand = 5 }, // 2
        Instruction{ .type = InstructionType::PRINT }, // 3
        Instruction{ .type = InstructionType::HALT }, // 4
        Instruction{ .type = InstructionType::PLUS }, // 5
        Instruction{ .type = InstructionType::RET } // 6
    };

    for (unsigned int i = 0; i < EXECUTION_LIMIT && !gVm.halt; i++) {
        Error error = gVm.executeInstruction();

        std::cout << "STACK:\n";
        if (gVm.stack.size() == 0) {
            std::cout << "\tempty\n";
        } else for (Word j : gVm.stack) {
            std::cout << "\t" << j << '\n';
        }

        if (error != Error::OK) {
            std::cerr << "ERROR: " << errorToString(error) << '\n';
            std::exit(1);
        }
    }

    std::cout << "\nExecution ended\n";

    return 0;
}