#include <Parser.hpp>

#include <Lexer.hpp>
#include <Token.hpp>

#include <format>
#include <fstream>
#include <iostream>
#include <regex>
#include <vector>

#define CMD(command)                                   \
    do {                                               \
        std::cout << "COMMAND: " << (command) << '\n'; \
        std::system((command).c_str());                \
    } while (0)

void Parser::parse()
{
    switch (this->target) {
    case EXSI:
        std::cout << "ERROR: Intrepreting is not supported!\n";
        std::exit(1);
        break;
    case NASM_LINUX_X86_64:
        this->compileToNasmLinux86_64();
        break;
    case NASM_WIN32:
        std::cout << "ERROR: Compiling to nasm for Win32 is not supported!\n";
        std::exit(1);
        break;
#ifdef SUPPORT_LLVM
    case LLVM:
        std::cout << "ERROR: Compiling to LLVM IR is not supported!\n";
        std::exit(1);
        break;
#endif
    }
}

void Parser::compileToNasmLinux86_64()
{
    std::string outputFilepath(std::regex_replace(inputFileName, std::regex("\\.xes$"), ".asm"));
    std::string output("BITS 64\n"
                       "extern printf\n"
                       "global _start\n"
                       "section .text\n"
                       "_start:\n");

    std::cout << "INFO: Generating " << outputFilepath << '\n';

    std::vector<std::string> stringStorage;

    for (; ip < this->program.size(); ip++) {
        Token token = this->program[ip];

        switch (token.type) {
        case PUSH: {
            output.append(std::format("addr_{}: ;; {}: PUSH {}\n", ip, token.pos.toString(), std::stoi(token.value.text)));
            output.append(std::format("    push {}\n", token.value.text));
        } break;
        case STRING: {
            output.append(std::format("addr_{}: ;; {}: STRING {}\n", ip, token.pos.toString(), token.value.text));
            output.append(std::format("    push str_{}\n", stringStorage.size()));
            stringStorage.push_back(token.value.text);
        } break;
        case STRING_DUMP: {
            output.append(std::format("addr_{}: ;; {}: STRING_DUMP\n", ip, token.pos.toString()));
            output.append("    pop rsi\n");
            output.append("    mov rdi, strPrintfFmt\n");
            output.append("    xor eax, eax\n");
            output.append("    call printf\n");
        } break;
        case STRING_PLUS: {
            output.append(std::format("addr_{}: ;; {}: STRING_PLUS\n", ip, token.pos.toString()));
            std::cout << "Strings is not implemented in nasm-linux-x86_64!\n";
            std::exit(1);
        } break;
        case DUP: {
            output.append(std::format("addr_{}: ;; {}: DUP\n", ip, token.pos.toString()));
            output.append("    pop rax\n");
            output.append("    push rax\n");
            output.append("    push rax\n");
        } break;
        case OVER: {
            output.append(std::format("addr_{}: ;; {}: OVER\n", ip, token.pos.toString()));
            output.append("    pop rax\n");
            output.append("    pop rbx\n");
            output.append("    push rbx\n");
            output.append("    push rax\n");
            output.append("    push rbx\n");
        } break;
        case DROP: {
            output.append(std::format("addr_{}: ;; {}: DROP\n", ip, token.pos.toString()));
            output.append("    pop rax\n");
        } break;
        case SWAP: {
            output.append(std::format("addr_{}: ;; {}: SWAP\n", ip, token.pos.toString()));
            output.append("    pop rax\n");
            output.append("    pop rbx\n");
            output.append("    push rax\n");
            output.append("    push rbx\n");
        } break;
        case PLUS: {
            output.append(std::format("addr_{}: ;; {}: PLUS\n", ip, token.pos.toString()));
            output.append("    pop rax\n");
            output.append("    pop rbx\n");
            output.append("    add rbx, rax\n");
            output.append("    push rbx\n");
        } break;
        case MINUS: {
            output.append(std::format("addr_{}: ;; {}: MINUS\n", ip, token.pos.toString()));
            output.append("    pop rax\n");
            output.append("    pop rbx\n");
            output.append("    sub rbx, rax\n");
            output.append("    push rbx\n");
        } break;
        case MULT: {
            output.append(std::format("addr_{}: ;; {}: MINUS\n", ip, token.pos.toString()));
            output.append("    pop rax\n");
            output.append("    pop rbx\n");
            output.append("    mul rbx\n");
            output.append("    push rax\n");
        } break;
        case DIV: {
            output.append(std::format("addr_{}: ;; {}: DIV\n", ip, token.pos.toString()));
            output.append("    pop rbx\n");
            output.append("    pop rax\n");
            output.append("    xor rdx, rdx\n");
            output.append("    div rbx\n");
            output.append("    push rax\n");
            output.append("    push rdx\n");
        } break;
        case DUMP: {
            output.append(std::format("addr_{}: ;; {}: DUMP\n", ip, token.pos.toString()));
            output.append("    pop rsi\n");
            output.append("    mov rdi, numPrintfFmt\n");
            output.append("    xor eax, eax\n");
            output.append("    call printf\n");
        } break;
        case INPUT: {
            output.append(std::format("addr_{}: ;; {}: INPUT\n", ip, token.pos.toString()));
            std::cout << "User input is not implemented in NASM_LINUX_X86_64 target!\n";
            std::exit(1);
        } break;
        case BIND: {
            output.append(std::format("addr_{}: ;; {}: BIND\n", ip, token.pos.toString()));
            std::cout << "Bindings is not implemented in NASM_LINUX_X86_64 target!\n";
            std::exit(1);
        } break;
        case SAVE: {
            output.append(std::format("addr_{}: ;; {}: SAVE\n", ip, token.pos.toString()));
            std::cout << "Bindings is not implemented in NASM_LINUX_X86_64 target!\n";
            std::exit(1);
        } break;
        case LOAD: {
            output.append(std::format("addr_{}: ;; {}: LOAD\n", ip, token.pos.toString()));
            std::cout << "Bindings is not implemented in NASM_LINUX_X86_64 target!\n";
            std::exit(1);
        } break;
        case TERNARY: {
            output.append(std::format("addr_{}: ;; {}: TERNARY\n", ip, token.pos.toString()));
        } break;
        case MAKEPROC: {
            output.append(std::format("addr_{}: ;; {}: MAKEPROC (to {})\n", ip, token.pos.toString(), token.pairIp));
            std::cout << "Procedures is not implemented in NASM_LINUX_X86_64 target!\n";
            std::exit(1);
        } break;
        case ENDPROC: {
            output.append(std::format("addr_{}: ;; {}: ENDPROC\n", ip, token.pos.toString()));
            std::cout << "Procedures is not implemented in NASM_LINUX_X86_64 target!\n";
            std::exit(1);
        } break;
        case INVOKEPROC: {
            output.append(std::format("addr_{}: ;; {}: INVOKEPROC\n", ip, token.pos.toString()));
            std::cout << "Procedures is not implemented in NASM_LINUX_X86_64 target!\n";
            std::exit(1);
        } break;
        case IF: {
            output.append(std::format("addr_{}: ;; {}: IF (to {})\n", ip, token.pos.toString(), token.pairIp));
            output.append("    pop rax\n");
            output.append("    test rax, rax\n");
            output.append(std::format("    jz addr_{}\n", token.pairIp));
        } break;
        case ENDIF: {
            output.append(std::format("addr_{}: ;; {}: ENDIF (from {})\n", ip, token.pos.toString(), token.pairIp));
        } break;
        case EQUAL: {
            output.append(std::format("addr_{}: ;; {}: EQUAL\n", ip, token.pos.toString()));
            output.append("    mov rcx, 0\n");
            output.append("    mov rdx, 1\n");
            output.append("    pop rax\n");
            output.append("    pop rbx\n");
            output.append("    cmp rax, rbx\n");
            output.append("    cmove rcx, rdx\n");
            output.append("    push rcx\n");
        } break;
        case NOTEQUAL: {
            output.append(std::format("addr_{}: ;; {}: NOTEQUAL\n", ip, token.pos.toString()));
            output.append("    mov rcx, 0\n");
            output.append("    mov rdx, 1\n");
            output.append("    pop rax\n");
            output.append("    pop rbx\n");
            output.append("    cmp rax, rbx\n");
            output.append("    cmovne rcx, rdx\n");
            output.append("    push rcx\n");
        } break;
        case LESS: {
            output.append(std::format("addr_{}: ;; {}: LESS\n", ip, token.pos.toString()));
            output.append("    mov rcx, 0\n");
            output.append("    mov rdx, 1\n");
            output.append("    pop rbx\n");
            output.append("    pop rax\n");
            output.append("    cmp rax, rbx\n");
            output.append("    cmovl rcx, rdx\n");
            output.append("    push rcx\n");
        } break;
        case LESSEQUAL: {
            output.append(std::format("addr_{}: ;; {}: LESSEQUAL\n", ip, token.pos.toString()));
            output.append("    mov rcx, 0\n");
            output.append("    mov rdx, 1\n");
            output.append("    pop rbx\n");
            output.append("    pop rax\n");
            output.append("    cmp rax, rbx\n");
            output.append("    cmovle rcx, rdx\n");
            output.append("    push rcx\n");
        } break;
        case GREATER: {
            output.append(std::format("addr_{}: ;; {}: GREATER\n", ip, token.pos.toString()));
            output.append("    mov rcx, 0\n");
            output.append("    mov rdx, 1\n");
            output.append("    pop rbx\n");
            output.append("    pop rax\n");
            output.append("    cmp rax, rbx\n");
            output.append("    cmovg rcx, rdx\n");
            output.append("    push rcx\n");
        } break;
        case GREATEREQUAL: {
            output.append(std::format("addr_{}: ;; {}: GREATEREQUAL\n", ip, token.pos.toString()));
            output.append("    mov rcx, 0\n");
            output.append("    mov rdx, 1\n");
            output.append("    pop rbx\n");
            output.append("    pop rax\n");
            output.append("    cmp rax, rbx\n");
            output.append("    cmovge rcx, rdx\n");
            output.append("    push rcx\n");
        } break;
        case LOR: {
            output.append(std::format("addr_{}: ;; {}: LOR\n", ip, token.pos.toString()));
            output.append("    pop rax\n");
            output.append("    pop rbx\n");
            output.append("    or rbx, rax\n");
            output.append("    push rbx\n");
        } break;
        case LAND: {
            output.append(std::format("addr_{}: ;; {}: LAND\n", ip, token.pos.toString()));
            output.append("    pop rax\n");
            output.append("    pop rbx\n");
            output.append("    and rbx, rax\n");
            output.append("    push rbx\n");
        } break;
        case LNOT: {
            output.append(std::format("addr_{}: ;; {}: LNOT\n", ip, token.pos.toString()));
            output.append("    pop rax\n");
            output.append("    not rax\n");
            output.append("    push rax\n");
        } break;
        case UNDEFINED:
        case TRUE:
        case FALSE: {
            std::cout << "UwU EndeyshentWabs made a fucky wucky!! A wittle fucko boingo!\n";
            std::exit(69);
        } break;
        } // switch (token.type)
    } // for

    output.append("    mov rax, 60\n");
    output.append("    mov rdi, 0\n");
    output.append("    syscall\n");

    output.append("section .data\n");
    {
        output.append("    numPrintfFmt: db '%d',0xA,0\n");
        output.append("    strPrintfFmt: db '%s',0xA,0\n");
        for (unsigned int i = 0; i < stringStorage.size(); i++) {
            output.append(std::format("    str_{}: db ", i));
            for (unsigned int j = 0; j < stringStorage[i].size(); j++) {
                if (j > 0) {
                    output.append(",");
                }
                output.append(std::format("{:d}", stringStorage[i][j]));
            }
            output.append(",0\n");
        }
    }
    output.append("section .bss\n");
    {
        // TODO: Populate the .bss
    }

#if 1
    {
        std::ofstream outputStream(outputFilepath.c_str(), std::ios::out | std::ios::binary);
        if (!outputStream.is_open()) {
            std::cout << "No stream!\n";
            std::exit(2);
        } else {
            outputStream << output.c_str();
        }
        outputStream.close();
    }
#endif

    CMD(std::format("nasm -felf64 {}", outputFilepath));
    CMD(std::format("ld -dynamic-linker /lib64/ld-linux-x86-64.so.2 -o {} {} -lc", std::regex_replace(outputFilepath, std::regex("\\.asm$"), ""), std::regex_replace(outputFilepath, std::regex("\\.asm$"), ".o")));
}
