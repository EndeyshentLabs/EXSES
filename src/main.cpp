#include "Lexer.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cassert>

#include <utils.hpp>
#include <Token.hpp>

#define push(prg, nu) do { prg.push_back(Token(PUSH, nu)); } while (0)
#define plus(prg) do { prg.push_back(Token(PLUS)); } while (0)
#define minus(prg) do { prg.push_back(Token(MINUS)); } while (0)
#define mult(prg) do { prg.push_back(Token(MULT)); } while (0)
#define div(prg) do { prg.push_back(Token(DIV)); } while (0)
#define dup(prg) do { prg.push_back(Token(DUP)); } while (0)
#define swap(prg) do { prg.push_back(Token(SWAP)); } while (0)
#define dump(prg) do { prg.push_back(Token(DUMP)); } while (0)

#if 0
void convertToPython3(std::vector<Token> program)
{
    std::string output;
    output.append("#!/usr/bin/env python3\nstack = []\n");
    std::vector<int> stack;

    for (auto op : program) {
        switch (op.type) {
            case PUSH: {
                output.append("stack.append(" + std::to_string(op.getValue()) + ")\n");
            } break;
            case DUP: {
                output.append("stack.append(stack[len(stack) - 1])\n");
            } break;
            case SWAP: {
                // WARNING: Weird code ahead!
                output.append("stack.append(stack.pop(0))\n");
                output.append("stack.append(stack.pop(1))\n");
            } break;
            case PLUS: {
                output.append("stack.append(stack.pop() + stack.pop())\n");
            } break;
            case MINUS: {
                output.append("stack.append(stack.pop(0) - stack.pop())\n");
            } break;
            case MULT: {
                output.append("stack.append(stack.pop() * stack.pop())\n");
            } break;
            case DIV: {
                output.append("stack.append(stack.pop() / stack.pop())\n");
            } break;
            case DUMP: {
                output.append("print(stack.pop())\n");
            } break;
        }
    }

    std::cout << output;
}
#endif

int main(int argc, char* argv[])
{
    Target target = EXSI;
    std::ifstream sourceFile;
    std::string sourceFileContents;

    // std::vector<Token> prg;
    // push(prg, 123);
    // push(prg, 7);
    // plus(prg);
    // push(prg, 30);
    // minus(prg);
    // dump(prg);
    // push(prg, 3);
    // dup(prg);
    // mult(prg);
    // dump(prg);

    if (argc < 2) {
        std::cerr << "ERROR: Not enough arguments is provided!\n";
        return 1;
    }


    sourceFile.open(argv[1], std::ios::in);
    if (sourceFile.is_open()) {
        std::string line;

        while (std::getline(sourceFile, line)) {
            sourceFileContents.append(line + '\n');
        }
        sourceFile.close();

        Lexer lexer(argv[1], sourceFileContents, target);
        lexer.intrepret();
    } else {
        std::cerr << "ERROR: Couldn't open a file '" << argv[1] << "'!\n";
        return 1;
    }

    // std::vector<std::string> words = split(sourceFileContents, "\\s");

    // for (const auto& word : words) {
    //     std::cout << word << '\n';
    // }

    return 0;
}
