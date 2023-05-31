#ifndef XESVM_XESVM_H

#include <vector>

#include <Instruction.hpp>
#include <Error.hpp>

class Xesvm {
public:
    std::vector<Word> stack;
    std::vector<Instruction> program;
    Word ip = 0;
    bool halt = false;

    Error executeInstruction();
};

extern Xesvm gVm;

#endif // XESVM_XESVM_H
