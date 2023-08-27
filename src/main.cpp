#include <Lexer.hpp>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

void header()
{
    std::cout << "EXSI - EXSES intrepreter and compiler.\n";
    std::cout << "Version 0.5.0-dev\n";
}

void usage(std::string program)
{
    header();
    std::cout << "Usage: " << program << " <TARGET> <file>\n";
    std::cout << "TARGETs:\n";
    std::cout << "\texsi             \tNative EXSES intrepreter. #NOTE: Not supported!\n";
    std::cout << "\tnasm-linux-x86_64\tCompile program to nasm for Linux x86_64 instruction set.\n";
    std::cout << "\tnasm-win32       \tCompile program to nasm for Win32 instruction set. #NOTE: Not supported!\n";
}

const char* argShift(int& argc, char**& argv)
{
    const char* result = *argv;

    argc--;
    argv++;

    return result;
}

int main(int argc, char** argv)
{
    Target trg = NASM_LINUX_X86_64;

    std::string program(argShift(argc, argv));

    if (argc < 1) {
        usage(program);
        std::cout << "ERROR: Target wasn't provided!\n";
        return 1;
    }

    std::string target(argShift(argc, argv));

    if (target == "exsi") {
        trg = EXSI;
    } else if (target == "nasm-linux-x86_64") {
        trg = NASM_LINUX_X86_64;
    } else if (target == "nasm-win32") {
        trg = NASM_WIN32;
    } else {
        usage(program);
        std::cout << "ERROR: Unknown target `" << target << "`\n";
        return 1;
    }

    if (argc < 1) {
        usage(program);
        std::cerr << "ERROR: No file was provided!\n";
        return 1;
    }

    std::string fileName(argShift(argc, argv));

    Lexer lexer(fileName, trg);

    return 0;
}
