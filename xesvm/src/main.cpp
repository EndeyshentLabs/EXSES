#include <iostream>
#include <vector>

#define EXECUTION_LIMIT 64 // TODO: tweak this value or remove

#include "Instruction.hpp"
#include "Error.hpp"
#define XESVM_IMPLEMENTATION
#include "Xesvm.hpp"

// !NOTE! This is just a concept !NOTE! //

// TODO: Assembly language for XESVM (xesasm?)
// TODO: XESVM bytecode

int main()
{
    Xesvm vm;
    vm.program = {                                                  // addr
        Instruction { .type = InstructionType::PUSH, .operand = 0}, // 0
        Instruction { .type = InstructionType::PUSH, .operand = 1}, // 1
        Instruction { .type = InstructionType::DUP, .operand = 1 }, // 2
        Instruction { .type = InstructionType::DUP, .operand = 1 }, // 3
        Instruction { .type = InstructionType::PLUS },              // 4
        Instruction { .type = InstructionType::JMP, .operand = 2 }  // 5
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
