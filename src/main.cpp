#include <Lexer.hpp>
#include <iostream>
#include <string>

void header()
{
    std::cout << "EXSI - EXSES intrepreter and compiler.\n";
    std::cout << "Version 0.6.0-dev\n";
}

void usage(std::string program)
{
    header();
    std::cout << "Usage: " << program << " [-t target] <file>\n";
    std::cout << "Available targets:\n";
    std::cout << "\texsi             \tNative EXSES intrepreter. Default option.\n";
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
    Target trg = EXSI;

    std::string program(argShift(argc, argv));

    if (argc < 1) {
        usage(program);
        std::cout << "ERROR: No input is provided!\n";
        return 1;
    }

    std::string arg1(argShift(argc, argv));
    std::string fileName;

    if (arg1 == "-t") {
        if (argc < 1) {
            usage(program);
            std::cerr << "ERROR: Target wasn't provided!\n";
            return 1;
        }

        std::string target = argShift(argc, argv);

        if (target == "exsi") {
            trg = EXSI;
        } else if (target == "nasm-linux-x86_64") {
            trg = NASM_LINUX_X86_64;
        } else if (target == "nasm-win32") {
            trg = NASM_WIN32;
        } else {
            usage(program);
            std::cerr << "ERROR: Unknown target " << target << ".\n";
            return 1;
        }

        if (argc < 1) {
            usage(program);
            std::cerr << "ERROR: Filename wasn't provided!\n";
            return 1;
        }

        fileName = argShift(argc, argv);
    } else {
        fileName = arg1;
    }

    Lexer lexer(fileName, trg);

    return 0;
}
