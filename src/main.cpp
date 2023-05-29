#include <Lexer.hpp>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

void header()
{
    std::cout << "EXSI - EXSES intrepreter and compiler.\n";
    std::cout << "Version 0.4.0-dev\n";
}

void usage(char** argv)
{
    header();
    std::cout << "Usage: " << argv[0] << " <file>\n";
    // std::cout << "Modes:\n";
    // std::cout << "    exsi    - native EXSES intrepreter.\n";
    // std::cout << "    python3 - compile EXSES code to Python 3.\n";
}

int main(int argc, char** argv)
{
    Target target = EXSI;

    if (argc < 2) {
        usage(argv);
        std::cerr << "ERROR: Not enough arguments is provided!\n";
        return 1;
    }

    // if (std::strcmp(argv[1], "exsi") == 0) {
    //     target = EXSI;
    // } else {
    //     usage(argv);
    //     std::cerr << "ERROR: Unknown mode `" << argv[1] << "`\n";
    // }

    Lexer lexer(argv[1], target);

    return 0;
}
