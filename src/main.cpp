#include <Lexer.hpp>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

void header()
{
    std::cout << "EXSI - EXSES intrepreter and compiler.\n";
    std::cout << "Version 0.2.0\n";
}

void usage(char** argv)
{
    header();
    std::cout << "Usage: " << argv[0] << " <mode> <file>\n";
    std::cout << "Modes:\n";
    std::cout << "    exsi    - native EXSES intrepreter.\n";
    std::cout << "    python3 - compile EXSES code to Python 3.\n";
}

int main(int argc, char** argv)
{
    Target target = EXSI;
    std::ifstream sourceFile;
    std::string sourceFileContents;

    if (argc < 3) {
        usage(argv);
        std::cerr << "ERROR: Not enough arguments is provided!\n";
        return 1;
    }

    if (std::strcmp(argv[1], "exsi") == 0) {
        target = EXSI;
    } else if (std::strcmp(argv[1], "python3") == 0) {
        target = PYTHON3;
    } else {
        usage(argv);
        std::cerr << "ERROR: Unknown mode `" << argv[1] << "`\n";
    }

    sourceFile.open(argv[2], std::ios::in);
    if (sourceFile.is_open()) {
        std::string line;

        while (std::getline(sourceFile, line)) {
            sourceFileContents.append(line + '\n');
        }
        sourceFile.close();

        Lexer lexer(argv[2], sourceFileContents, target);
        lexer.run();
    } else {
        std::cerr << "ERROR: Couldn't open a file '" << argv[2] << "'!\n";
        return 1;
    }

    return 0;
}
