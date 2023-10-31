#include "Value.hpp"
#include <Parser.hpp>

#include <Lexer.hpp>
#include <Token.hpp>

#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <vector>

#include <fmt/color.h>
#include <fmt/format.h>

#define CMD(command)                                   \
    do {                                               \
        std::cout << "COMMAND: " << (command) << '\n'; \
        std::system((command).c_str());                \
    } while (0)

struct BindingInfo {
    unsigned long size;
    InstructionAddr addr;
};

std::map<std::string, InstructionAddr> procs;
std::map<std::string, BindingInfo> bindings;

void Parser::parse()
{
    switch (this->target) {
    case EXSI:
        this->intrepret();
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
                       // "extern puts\n"
                       "global _start\n"
                       "section .text\n"
                       "stringLenght:\n"
                       "    mov rbx, 0\n"
                       ".loop:\n"
                       "    cmp BYTE [rax + rbx], 0\n"
                       "    je .end\n"
                       "    inc rbx\n"
                       "    jmp .loop\n"
                       ".end:\n"
                       "    ret\n"
                       "_start:\n"
                       "    mov rax, saved_rsp_end\n"
                       "    mov [saved_rsp], rax\n");

    std::cout << "INFO: Generating " << outputFilepath << '\n';

    std::vector<std::string> stringStorage;

    for (; ip < this->program.size(); ip++) {
        Token token = this->program[ip];

        switch (token.type) {
        case PUSH: {
            output.append(fmt::format("addr_{}: ;; {}: PUSH {}\n", ip, token.pos.toString(), token.value.text));
            output.append(fmt::format("    mov r11, {}\n", token.value.text));
            output.append("    push r11\n");
        } break;
        case STRING: {
            output.append(fmt::format("addr_{}: ;; {}: STRING {}\n", ip, token.pos.toString(), token.text));
            output.append(fmt::format("    push str_{}\n", stringStorage.size()));
            stringStorage.push_back(token.value.text);
        } break;
        case STRING_DUMP: {
            output.append(fmt::format("addr_{}: ;; {}: STRING_DUMP\n", ip, token.pos.toString()));
            output.append("    pop r11\n");
            output.append("    mov rax, r11\n");
            output.append("    call stringLenght\n");
            output.append("    mov rdx, rbx\n");
            output.append("    mov rax, 1\n");
            output.append("    mov rdi, 1\n");
            output.append("    mov rsi, r11\n");
            output.append("    syscall\n");
        } break;
        case STRING_PLUS: {
            output.append(fmt::format("addr_{}: ;; {}: STRING_PLUS\n", ip, token.pos.toString()));
            error(token, "String-plus operation is not implemented in nasm-linux-x86_64");
        } break;
        case DUP: {
            output.append(fmt::format("addr_{}: ;; {}: DUP\n", ip, token.pos.toString()));
            output.append("    pop rax\n");
            output.append("    push rax\n");
            output.append("    push rax\n");
        } break;
        case OVER: {
            output.append(fmt::format("addr_{}: ;; {}: OVER\n", ip, token.pos.toString()));
            output.append("    pop rax\n");
            output.append("    pop rbx\n");
            output.append("    push rbx\n");
            output.append("    push rax\n");
            output.append("    push rbx\n");
        } break;
        case DROP: {
            output.append(fmt::format("addr_{}: ;; {}: DROP\n", ip, token.pos.toString()));
            output.append("    pop r11\n");
        } break;
        case SWAP: {
            output.append(fmt::format("addr_{}: ;; {}: SWAP\n", ip, token.pos.toString()));
            output.append("    pop rax\n");
            output.append("    pop rbx\n");
            output.append("    push rax\n");
            output.append("    push rbx\n");
        } break;
        case PLUS: {
            output.append(fmt::format("addr_{}: ;; {}: PLUS\n", ip, token.pos.toString()));
            output.append("    pop rax\n");
            output.append("    pop rbx\n");
            output.append("    add rbx, rax\n");
            output.append("    push rbx\n");
        } break;
        case MINUS: {
            output.append(fmt::format("addr_{}: ;; {}: MINUS\n", ip, token.pos.toString()));
            output.append("    pop rax\n");
            output.append("    pop rbx\n");
            output.append("    sub rbx, rax\n");
            output.append("    push rbx\n");
        } break;
        case MULT: {
            output.append(fmt::format("addr_{}: ;; {}: MINUS\n", ip, token.pos.toString()));
            output.append("    pop rax\n");
            output.append("    pop rbx\n");
            output.append("    mul rbx\n");
            output.append("    push rax\n");
        } break;
        case DIV: {
            output.append(fmt::format("addr_{}: ;; {}: DIV\n", ip, token.pos.toString()));
            output.append("    pop rbx\n");
            output.append("    pop rax\n");
            output.append("    xor rdx, rdx\n");
            output.append("    div rbx\n");
            output.append("    push rax\n");
            output.append("    push rdx\n");
        } break;
        case DUMP: {
            output.append(fmt::format("addr_{}: ;; {}: DUMP\n", ip, token.pos.toString()));
            output.append("    pop rsi\n");
            output.append("    mov rdi, numPrintfFmt\n");
            output.append("    xor eax, eax\n");
            output.append("    call printf\n");
        } break;
        case INPUT: {
            output.append(fmt::format("addr_{}: ;; {}: INPUT\n", ip, token.pos.toString()));
            error(token, "User input is not implemented in NASM_LINUX_X86_64 target");
        } break;
        case BIND: {
            output.append(fmt::format("addr_{}: ;; {}: BIND\n", ip, token.pos.toString()));

            if ((int)ip - 1 < 0) {
                error(token, "Expected binding name as IDENT but got nothing");
                break;
            }

            Token& name = this->program.at(ip - 1);

            if (name.type != IDENT) {
                error(token, fmt::format("Expected binding name as IDENT but got {}", TokenTypeString[name.type]));
                break;
            }

            if (ip + 1 > this->program.size() - 1) {
                error(token, "Expected binding size as constexpr but got nothing");
                break;
            }

            Token& sizeConstExpr = this->program.at(ip + 1);

            if (sizeConstExpr.type != MAKECONSTEXPR) {
                error(token, fmt::format("Expected binding name as constexpr but got {}", TokenTypeString[sizeConstExpr.type]));
                break;
            }

            Value sizeValue = evalConstExpr(ip + 1, sizeConstExpr.pairIp);

            if (sizeValue.text[0] == '-') {
                error(token, "Binding size expected to be positive but got negative");
                note(this->program[ip + 1], "In this evaluated constant expression");
                break;
            }

            unsigned long size = std::stoul(sizeValue.text);

            if (bindings.contains(name.value.text)) {
                error(token, fmt::format("Binding with the name '{}' was alreay defined", name.value.text));
                note(this->program[procs.at(name.value.text)], "Defined here");
                break;
            } else if (procs.contains(name.value.text)) {
                error(token, fmt::format("Procedure with the same name exist", name.value.text));
                note(this->program[procs.at(name.value.text)], "Defined here");
                break;
            }

            bindings.insert({ name.value.text, BindingInfo(size, ip) });
        } break;
        case SAVE: {
            output.append(fmt::format("addr_{}: ;; {}: SAVE\n", ip, token.pos.toString()));

            output.append("    pop r11\n");
            output.append("    pop r12\n");
            output.append("    mov [r11], r12\n");
        } break;
        case LOAD8: {
            output.append(fmt::format("addr_{}: ;; {}: LOAD8\n", ip, token.pos.toString()));

            output.append("    pop r11\n");
            output.append("    xor r12, r12\n");
            output.append("    mov r12b, [r11]\n");
            output.append("    push r12\n");
        } break;
        case LOAD16: {
            output.append(fmt::format("addr_{}: ;; {}: LOAD16\n", ip, token.pos.toString()));

            output.append("    pop r11\n");
            output.append("    xor r12, r12\n");
            output.append("    mov r12w, [r11]\n");
            output.append("    push r12\n");
        } break;
        case LOAD32: {
            output.append(fmt::format("addr_{}: ;; {}: LOAD32\n", ip, token.pos.toString()));

            output.append("    pop r11\n");
            output.append("    xor r12, r12\n");
            output.append("    mov r12d, [r11]\n");
            output.append("    push r12\n");
        } break;
        case LOAD64: {
            output.append(fmt::format("addr_{}: ;; {}: LOAD64\n", ip, token.pos.toString()));

            output.append("    pop r11\n");
            output.append("    xor r12, r12\n");
            output.append("    mov r12, [r11]\n");
            output.append("    push r12\n");
        } break;
        case TERNARY: {
            output.append(fmt::format("addr_{}: ;; {}: TERNARY\n", ip, token.pos.toString()));
            error(token, "Ternary operator is not implemented in NASM_LINUX_X86_64 target");
        } break;
        case MAKEPROC: {
            if ((int)ip - 1 < 0) {
                error(token, "Expected function name as IDENT but got nothing");
                break;
            }

            Token& name = this->program.at(ip - 1);

            if (name.type != IDENT) {
                error(token, fmt::format("Expected function name as IDENT but got {}", TokenTypeString[name.type]));
                break;
            }

            if (procs.contains(name.value.text)) {
                error(token, fmt::format("Procedure with the name '{}' was alreay defined", name.value.text));
                note(this->program[procs.at(name.value.text)], "Defined here");
                break;
            } else if (bindings.contains(name.value.text)) {
                error(token, fmt::format("Binding with the same name exist", name.value.text));
                note(this->program[procs.at(name.value.text)], "Defined here");
                break;
            }

            procs.insert({ name.value.text, ip });

            output.append(";; PRE-MAKEPROC\n");
            output.append(fmt::format("    jmp addr_{}\n", token.pairIp));
            output.append(fmt::format("addr_{}: ;; {}: MAKEPROC (to {})\n", ip, token.pos.toString(), token.pairIp));
            output.append("    sub rsp, 0\n");
            output.append("    mov [saved_rsp], rsp\n");
            output.append("    mov rsp, rax\n");
        } break;
        case ENDPROC: {
            output.append(";; PRE-ENDPROC\n");
            output.append("    mov rax, rsp\n");
            output.append("    mov rsp, [saved_rsp]\n");
            output.append("    add rsp, 0\n");
            output.append("    ret\n");
            output.append(fmt::format("addr_{}: ;; {}: ENDPROC\n", ip, token.pos.toString()));
        } break;
        case INVOKEPROC: {
            output.append(fmt::format("addr_{}: ;; {}: INVOKEPROC\n", ip, token.pos.toString()));
            if ((int)ip - 1 < 0) {
                error(token, "Expected function name as IDENT but got nothing");
                break;
            }

            Token& name = this->program.at(ip - 1);

            if (name.type != IDENT) {
                error(token, fmt::format("Expected function name as IDENT but got {}", TokenTypeString[name.type]));
                break;
            }

            if (!procs.contains(name.value.text)) {
                error(token, fmt::format("Procedure '{}' is not defined", name.value.text));
                break;
            }

            output.append("    mov rax, rsp\n");
            output.append("    mov rsp, [saved_rsp]\n");
            output.append(fmt::format("    call addr_{}\n", procs.at(name.value.text)));
            output.append("    mov [saved_rsp], rsp\n");
            output.append("    mov rsp, rax\n");
        } break;
        case IF: {
            output.append(fmt::format("addr_{}: ;; {}: IF (to {})\n", ip, token.pos.toString(), token.pairIp));
            output.append("    pop rax\n");
            output.append("    test rax, rax\n");
            output.append(fmt::format("    jz addr_{}\n", token.pairIp));
        } break;
        case ENDIF: {
            output.append(fmt::format("addr_{}: ;; {}: ENDIF (from {})\n", ip, token.pos.toString(), token.pairIp));
        } break;
        case WHILE: {
            output.append(fmt::format("addr_{}: ;; {}: WHILE (from {})\n", ip, token.pos.toString(), token.pairIp));
        } break;
        case DOWHILE: {
            output.append(fmt::format("addr_{}: ;; {}: DOWHILE (to {})\n", ip, token.pos.toString(), token.pairIp));
            output.append("    pop rax\n");
            output.append("    test rax, rax\n");
            output.append(fmt::format("    jz addr_{}\n", token.pairIp));
        } break;
        case ENDWHILE: {
            output.append("    ;; Pre-ENDWHILE\n");
            output.append(fmt::format("    jmp addr_{}\n", token.pairIp));
            output.append(fmt::format("addr_{}: ;; {}: ENDWHILE (to {})\n", ip, token.pos.toString(), token.pairIp));
        } break;
        case EQUAL: {
            output.append(fmt::format("addr_{}: ;; {}: EQUAL\n", ip, token.pos.toString()));
            output.append("    mov rcx, 0\n");
            output.append("    mov rdx, 1\n");
            output.append("    pop rax\n");
            output.append("    pop rbx\n");
            output.append("    cmp rax, rbx\n");
            output.append("    cmove rcx, rdx\n");
            output.append("    push rcx\n");
        } break;
        case NOTEQUAL: {
            output.append(fmt::format("addr_{}: ;; {}: NOTEQUAL\n", ip, token.pos.toString()));
            output.append("    mov rcx, 0\n");
            output.append("    mov rdx, 1\n");
            output.append("    pop rax\n");
            output.append("    pop rbx\n");
            output.append("    cmp rax, rbx\n");
            output.append("    cmovne rcx, rdx\n");
            output.append("    push rcx\n");
        } break;
        case LESS: {
            output.append(fmt::format("addr_{}: ;; {}: LESS\n", ip, token.pos.toString()));
            output.append("    mov rcx, 0\n");
            output.append("    mov rdx, 1\n");
            output.append("    pop rbx\n");
            output.append("    pop rax\n");
            output.append("    cmp rax, rbx\n");
            output.append("    cmovl rcx, rdx\n");
            output.append("    push rcx\n");
        } break;
        case LESSEQUAL: {
            output.append(fmt::format("addr_{}: ;; {}: LESSEQUAL\n", ip, token.pos.toString()));
            output.append("    mov rcx, 0\n");
            output.append("    mov rdx, 1\n");
            output.append("    pop rbx\n");
            output.append("    pop rax\n");
            output.append("    cmp rax, rbx\n");
            output.append("    cmovle rcx, rdx\n");
            output.append("    push rcx\n");
        } break;
        case GREATER: {
            output.append(fmt::format("addr_{}: ;; {}: GREATER\n", ip, token.pos.toString()));
            output.append("    mov rcx, 0\n");
            output.append("    mov rdx, 1\n");
            output.append("    pop rbx\n");
            output.append("    pop rax\n");
            output.append("    cmp rax, rbx\n");
            output.append("    cmovg rcx, rdx\n");
            output.append("    push rcx\n");
        } break;
        case GREATEREQUAL: {
            output.append(fmt::format("addr_{}: ;; {}: GREATEREQUAL\n", ip, token.pos.toString()));
            output.append("    mov rcx, 0\n");
            output.append("    mov rdx, 1\n");
            output.append("    pop rbx\n");
            output.append("    pop rax\n");
            output.append("    cmp rax, rbx\n");
            output.append("    cmovge rcx, rdx\n");
            output.append("    push rcx\n");
        } break;
        case LOR: {
            output.append(fmt::format("addr_{}: ;; {}: LOR\n", ip, token.pos.toString()));
            output.append("    pop rax\n");
            output.append("    pop rbx\n");
            output.append("    or rbx, rax\n");
            output.append("    push rbx\n");
        } break;
        case LAND: {
            output.append(fmt::format("addr_{}: ;; {}: LAND\n", ip, token.pos.toString()));
            output.append("    pop rax\n");
            output.append("    pop rbx\n");
            output.append("    and rbx, rax\n");
            output.append("    push rbx\n");
        } break;
        case LNOT: {
            output.append(fmt::format("addr_{}: ;; {}: LNOT\n", ip, token.pos.toString()));
            output.append("    pop rax\n");
            output.append("    not rax\n");
            output.append("    push rax\n");
        } break;
        case TORAX: {
            output.append(fmt::format("addr_{}: ;; {}: TORAX\n", ip, token.pos.toString()));
            output.append("    pop rax\n");
        } break;
        case TORBX: {
            output.append(fmt::format("addr_{}: ;; {}: TORBX\n", ip, token.pos.toString()));
            output.append("    pop rbx\n");
        } break;
        case TORCX: {
            output.append(fmt::format("addr_{}: ;; {}: TORCX\n", ip, token.pos.toString()));
            output.append("    pop rcx\n");
        } break;
        case TORDX: {
            output.append(fmt::format("addr_{}: ;; {}: TORDX\n", ip, token.pos.toString()));
            output.append("    pop rdx\n");
        } break;
        case TORSI: {
            output.append(fmt::format("addr_{}: ;; {}: TORSI\n", ip, token.pos.toString()));
            output.append("    pop rsi\n");
        } break;
        case TORDI: {
            output.append(fmt::format("addr_{}: ;; {}: TORDI\n", ip, token.pos.toString()));
            output.append("    pop rdi\n");
        } break;
        case TORBP: {
            output.append(fmt::format("addr_{}: ;; {}: TORBP\n", ip, token.pos.toString()));
            output.append("    pop rbp\n");
        } break;
        case TOR8: {
            output.append(fmt::format("addr_{}: ;; {}: TOR8\n", ip, token.pos.toString()));
            output.append("    pop r8\n");
        } break;
        case TOR9: {
            output.append(fmt::format("addr_{}: ;; {}: TOR9\n", ip, token.pos.toString()));
            output.append("    pop r9\n");
        } break;
        case TOR10: {
            output.append(fmt::format("addr_{}: ;; {}: TOR10\n", ip, token.pos.toString()));
            output.append("    pop r10\n");
        } break;
        case SYSTEM_SYSCALL: {
            output.append(fmt::format("addr_{}: ;; {}: SYSCALL\n", ip, token.pos.toString()));
            output.append("    syscall\n");
            output.append("    push rax\n");
        } break;
        case IDENT: {
            if ((ip + 1 <= this->program.size() - 1 && (this->program[ip + 1].type == INVOKEPROC || this->program[ip + 1].type == MAKEPROC || this->program[ip + 1].type == BIND)))
                break;

            if (bindings.contains(token.value.text)) {
                output.append(fmt::format("addr_{}: ;; {}: IDENT as LOAD\n", ip, token.pos.toString()));

                output.append(fmt::format("    mov r11, binding_{}\n", bindings.at(token.value.text).addr));
                output.append("    push r11\n");
            } else {
                error(token, "Unexpected IDENT");
            }
        } break;
        case MAKECONSTEXPR: {
            if ((int)ip - 1 < 0 || this->program[ip - 1].type != BIND) {
                error(token, "Unexpected evaluated constant expression");
            }
        } break;
        case ENDCONSTEXPR: {
            if ((int)token.pairIp - 1 < 0 || this->program[token.pairIp - 1].type != BIND) {
                error(token, "Unexpected evaluated constant expression");
            }
        } break;
        case TRUE: {
            output.append(fmt::format("addr_{}: ;; {}: TRUE\n", ip, token.pos.toString()));
            output.append("    push 1");
        } break;
        case FALSE: {
            output.append(fmt::format("addr_{}: ;; {}: TRUE\n", ip, token.pos.toString()));
            output.append("    push 0");
        } break;
        case UNDEFINED: {
            std::cout << fmt::format("{} UwU ewwow: EndeyshentWabs made a fucky wucky!! A wittle fucko boingo!\n", token.pos.toString());
            std::exit(69);
        } break;
        } // switch (token.type)

        if (this->hadError) {
            std::cout << "INFO: Compilation \033[31;1mterminated\033[0m\n";
            std::exit(1);
        }
    } // for

    output.append("    ;; End of program\n");
    output.append("    mov rax, 60\n");
    output.append("    mov rdi, 0\n");
    output.append("    syscall\n");

    output.append("section .data\n");
    {
        output.append("    numPrintfFmt: db '%d',0xA,0\n");
        for (unsigned int i = 0; i < stringStorage.size(); i++) {
            output.append(fmt::format("    str_{}: db ", i));
            for (unsigned int j = 0; j < stringStorage[i].size(); j++) {
                if (j > 0) {
                    output.append(",");
                }
                output.append(fmt::format("{:d}", stringStorage[i][j]));
            }
            output.append(",0\n");
        }
    }
    output.append("section .bss\n");
    {
        // Inspired by https://gitlab.com/tsoding/porth
        output.append("    saved_rsp: resq 1\n");
        output.append("    saved_rsp_offset: resb 65536\n");
        output.append("    saved_rsp_end:\n");

        for (auto const& [name, info] : bindings) {
            output.append(fmt::format("    binding_{}: resb {}\n", info.addr, info.size));
        }
    }

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

    // TODO: Should stop compilation if failed
    CMD(fmt::format("nasm -g -felf64 {}", outputFilepath));
    CMD(fmt::format("ld -dynamic-linker /lib64/ld-linux-x86-64.so.2 -o {} {} -lc", std::regex_replace(outputFilepath, std::regex("\\.asm$"), ""), std::regex_replace(outputFilepath, std::regex("\\.asm$"), ".o")));
}

void Parser::intrepret()
{
    std::vector<InstructionAddr> returnAddrStack;
    std::vector<Value> stack;

    while (ip < program.size()) {
        Token& token = this->program[ip];

        switch (token.type) {
        case PUSH: {
            stack.push_back(Value(ValueType::INT, token.value.text));

            ip++;
        } break;
        case STRING: {
            stack.push_back(Value(ValueType::STRING, token.value.text));

            ip++;
        } break;
        case DUMP:
        case STRING_DUMP: {
            Value a = stack.back();
            stack.pop_back();

            std::cout << a.text << (a.type == ValueType::INT ? "\n" : "");

            ip++;
        } break;
        case STRING_PLUS: {
            error(token, "STRING_PLUS is not implemented yet");
        } break;
        case DUP: {
            stack.push_back(stack.back());

            ip++;
        } break;
        case OVER: {
            Value top = stack.back();
            stack.pop_back();
            Value over = stack.back();
            stack.pop_back();

            stack.push_back(over);
            stack.push_back(top);
            stack.push_back(over);

            ip++;
        } break;
        case DROP: {
            stack.pop_back();

            ip++;
        } break;
        case SWAP: {
            Value top = stack.back();
            stack.pop_back();
            Value over = stack.back();
            stack.pop_back();

            stack.push_back(top);
            stack.push_back(over);

            ip++;
        } break;
        case PLUS: {
            Value top = stack.back();
            stack.pop_back();
            Value over = stack.back();
            stack.pop_back();

            stack.push_back(Value(ValueType::INT, std::to_string(std::stol(top.text) + std::stol(over.text))));

            ip++;
        } break;
        case MINUS: {
            Value top = stack.back();
            stack.pop_back();
            Value over = stack.back();
            stack.pop_back();

            stack.push_back(Value(ValueType::INT, std::to_string(std::stol(over.text) - std::stol(top.text))));

            ip++;
        } break;
        case MULT: {
            Value top = stack.back();
            stack.pop_back();
            Value over = stack.back();
            stack.pop_back();

            stack.push_back(Value(ValueType::INT, std::to_string(std::stol(top.text) * std::stol(over.text))));

            ip++;
        } break;
        case DIV: {
            Value top = stack.back();
            stack.pop_back();
            Value over = stack.back();
            stack.pop_back();

            stack.push_back(Value(ValueType::INT, std::to_string(std::stol(over.text) / std::stol(top.text))));

            ip++;
        } break;
        case INPUT: {
            error(token, "INPUT is not implemented yet");

            ip++;
        } break;
        case BIND: {
            error(token, "Bindings are not implemented yet");

            ip++;
        } break;
        case SAVE: {
            error(token, "Bindings are not implemented yet");

            ip++;
        } break;
        case LOAD8: {
            error(token, "Bindings are not implemented yet");

            ip++;
        } break;
        case LOAD16: {
            error(token, "Bindings are not implemented yet");

            ip++;
        } break;
        case LOAD32: {
            error(token, "Bindings are not implemented yet");

            ip++;
        } break;
        case LOAD64: {
            error(token, "Bindings are not implemented yet");

            ip++;
        } break;
        case MAKECONSTEXPR: {
            error(token, "Evaluated constant expressions are not implemented yet");

            ip++;
        } break;
        case ENDCONSTEXPR: {
            error(token, "Evaluated constant expressions are not implemented yet");

            ip++;
        } break;
        case TERNARY: {
            error(token, "Ternary operator is not implemented yet");

            ip++;
        } break;
        case MAKEPROC: {
            if ((int)ip - 1 < 0) {
                error(token, "Expected function name as IDENT but got nothing");
                break;
            }

            Token& name = this->program.at(ip - 1);

            if (name.type != IDENT) {
                error(token, fmt::format("Expected function name as IDENT but got {}", TokenTypeString[name.type]));
                break;
            }

            if (procs.contains(name.value.text)) {
                error(token, fmt::format("Procedure with the name '{}' was alreay defined", name.value.text));
                note(this->program[procs.at(name.value.text)], "Defined here");
                break;
            } else if (bindings.contains(name.value.text)) {
                error(token, fmt::format("Binding with the same name exist", name.value.text));
                note(this->program[procs.at(name.value.text)], "Defined here");
                break;
            }

            procs.insert({ name.value.text, ip });

            ip = token.pairIp + 1;
        } break;
        case ENDPROC: {
            ip = returnAddrStack.back() + 1;
            returnAddrStack.pop_back();
        } break;
        case INVOKEPROC: {
            if ((int)ip - 1 < 0) {
                error(token, "Expected function name as IDENT but got nothing");
                break;
            }

            Token& name = this->program.at(ip - 1);

            if (name.type != IDENT) {
                error(token, fmt::format("Expected function name as IDENT but got {}", TokenTypeString[name.type]));
                break;
            }

            if (!procs.contains(name.value.text)) {
                error(token, fmt::format("Procedure '{}' is not defined", name.value.text));
                break;
            }

            returnAddrStack.push_back(ip);
            ip = procs.at(name.value.text) + 1;
        } break;
        case IF: {
            Value cond = stack.back();
            stack.pop_back();

            if (!std::stol(cond.text)) {
                ip = token.pairIp + 1;
            } else {
                ip++;
            }
        } break;
        case ENDIF: {
            ip++;
        } break;
        case WHILE: {
            ip++;
        } break;
        case DOWHILE: {
            Value cond = stack.back();
            stack.pop_back();

            if (!std::stol(cond.text)) {
                ip = token.pairIp + 1;
            } else {
                ip++;
            }
        } break;
        case ENDWHILE: {
            ip = token.pairIp;
        } break;
        case EQUAL: {
            Value top = stack.back();
            stack.pop_back();
            Value over = stack.back();
            stack.pop_back();

            stack.push_back(Value(ValueType::INT, std::to_string(top.text == over.text)));

            ip++;
        } break;
        case NOTEQUAL: {
            Value top = stack.back();
            stack.pop_back();
            Value over = stack.back();
            stack.pop_back();

            stack.push_back(Value(ValueType::INT, std::to_string(top.text != over.text)));

            ip++;
        } break;
        case LESS: {
            Value top = stack.back();
            stack.pop_back();
            Value over = stack.back();
            stack.pop_back();

            stack.push_back(Value(ValueType::INT, std::to_string(std::stol(over.text) < std::stol(top.text))));

            ip++;
        } break;
        case LESSEQUAL: {
            Value top = stack.back();
            stack.pop_back();
            Value over = stack.back();
            stack.pop_back();

            stack.push_back(Value(ValueType::INT, std::to_string(std::stol(over.text) <= std::stol(top.text))));

            ip++;
        } break;
        case GREATER: {
            Value top = stack.back();
            stack.pop_back();
            Value over = stack.back();
            stack.pop_back();

            stack.push_back(Value(ValueType::INT, std::to_string(std::stol(over.text) > std::stol(top.text))));

            ip++;
        } break;
        case GREATEREQUAL: {
            Value top = stack.back();
            stack.pop_back();
            Value over = stack.back();
            stack.pop_back();

            stack.push_back(Value(ValueType::INT, std::to_string(std::stol(over.text) >= std::stol(top.text))));

            ip++;
        } break;
        case LOR: {
            Value top = stack.back();
            stack.pop_back();
            Value over = stack.back();
            stack.pop_back();

            stack.push_back(Value(ValueType::INT, std::to_string(std::stol(over.text) || std::stol(top.text))));

            ip++;
        } break;
        case LAND: {
            Value top = stack.back();
            stack.pop_back();
            Value over = stack.back();
            stack.pop_back();

            stack.push_back(Value(ValueType::INT, std::to_string(std::stol(over.text) && std::stol(top.text))));

            ip++;
        } break;
        case LNOT: {
            Value top = stack.back();
            stack.pop_back();

            stack.push_back(Value(ValueType::INT, std::to_string(!std::stol(top.text))));

            ip++;
        } break;
        case TRUE: {
            stack.push_back(Value(ValueType::INT, "1"));

            ip++;
        } break;
        case FALSE: {
            stack.push_back(Value(ValueType::INT, "0"));

            ip++;
        } break;
        case TORAX:
        case TORBX:
        case TORCX:
        case TORDX:
        case TORSI:
        case TORDI:
        case TORBP:
        case TOR8:
        case TOR9:
        case TOR10: {
            error(token, "Register manipulation is not available in Intrepretaton mode");
        } break;
        case SYSTEM_SYSCALL: {
            error(token, "Syscalls is not available int Intrepretaton mode");
        } break;
        case IDENT: {
            // error(token, "IDENTs are not implemented yet");

            ip++;
        } break;
        case UNDEFINED: {
            std::cout << fmt::format("{} UwU ewwow: EndeyshentWabs made a fucky wucky!! A wittle fucko boingo!\n", token.pos.toString());
            std::exit(69);
        } break;
        } // switch (token.type)

        if (this->hadError) {
            std::cout << "INFO: Intrepretaton \033[31;1mterminated\033[0m\n";
            std::exit(1);
        }
    } // while
}

Value Parser::evalConstExpr(InstructionAddr start, InstructionAddr end)
{
    std::vector<Token> exprProgram(this->program.begin() + start + 1, this->program.begin() + end);
    std::vector<Value> stack;

    if (exprProgram.size() < 1) {
        error(this->program[start], "Empty evaluated constant expression");
        return Value(ValueType::NONE, "");
    }

    for (Token& token : exprProgram) {
        switch (token.type) {
        case PUSH: {
            stack.push_back(token.value);
        } break;
        case DUP: {
            stack.push_back(stack.back());
        } break;
        case OVER: {
            Value top = stack.back();
            stack.pop_back();
            Value over = stack.back();
            stack.pop_back();

            stack.push_back(over);
            stack.push_back(top);
            stack.push_back(over);
        } break;
        case DROP: {
            stack.pop_back();
        } break;
        case SWAP: {
            Value top = stack.back();
            stack.pop_back();
            Value over = stack.back();
            stack.pop_back();

            stack.push_back(top);
            stack.push_back(over);
        } break;
        case PLUS: {
            Value top = stack.back();
            stack.pop_back();
            Value over = stack.back();
            stack.pop_back();

            if (top.type != ValueType::INT) {
                error(token, "Second argument of PLUS operataion expected to be INT");
                note(this->program[start], "In this evaluated constant expression");
                return Value(ValueType::NONE, "");
            }
            if (over.type != ValueType::INT) {
                error(token, "First argument of PLUS operataion expected to be INT");
                note(this->program[start], "In this evaluated constant expression");
                return Value(ValueType::NONE, "");
            }

            stack.push_back(Value(ValueType::INT, std::to_string(std::stol(over.text) + std::stol(top.text))));
        } break;
        case MINUS: {
            Value top = stack.back();
            stack.pop_back();
            Value over = stack.back();
            stack.pop_back();

            if (top.type != ValueType::INT) {
                error(token, "Second argument of MINUS operataion expected to be INT");
                note(this->program[start], "In this evaluated constant expression");
                return Value(ValueType::NONE, "");
            }
            if (over.type != ValueType::INT) {
                error(token, "First argument of MINUS operataion expected to be INT");
                note(this->program[start], "In this evaluated constant expression");
                return Value(ValueType::NONE, "");
            }

            stack.push_back(Value(ValueType::INT, std::to_string(std::stol(over.text) - std::stol(top.text))));
        } break;
        case MULT: {
            Value top = stack.back();
            stack.pop_back();
            Value over = stack.back();
            stack.pop_back();

            if (top.type != ValueType::INT) {
                error(token, "Second argument of MULT operataion expected to be INT");
                note(this->program[start], "In this evaluated constant expression");
                return Value(ValueType::NONE, "");
            }
            if (over.type != ValueType::INT) {
                error(token, "First argument of MULT operataion expected to be INT");
                note(this->program[start], "In this evaluated constant expression");
                return Value(ValueType::NONE, "");
            }

            stack.push_back(Value(ValueType::INT, std::to_string(std::stol(over.text) * std::stol(top.text))));
        } break;
        case DIV: {
            Value top = stack.back();
            stack.pop_back();
            Value over = stack.back();
            stack.pop_back();

            if (top.type != ValueType::INT) {
                error(token, "Second argument of DIV operataion expected to be INT");
                note(this->program[start], "In this evaluated constant expression");
                return Value(ValueType::NONE, "");
            }
            if (over.type != ValueType::INT) {
                error(token, "First argument of DIV operataion expected to be INT");
                note(this->program[start], "In this evaluated constant expression");
                return Value(ValueType::NONE, "");
            }

            stack.push_back(Value(ValueType::INT, std::to_string(std::stol(over.text) / std::stol(top.text))));
        } break;
        case LOR: {
            Value top = stack.back();
            stack.pop_back();
            Value over = stack.back();
            stack.pop_back();

            if (top.type != ValueType::INT) {
                error(token, "Second argument of LOR operataion expected to be INT");
                note(this->program[start], "In this evaluated constant expression");
                return Value(ValueType::NONE, "");
            }
            if (over.type != ValueType::INT) {
                error(token, "First argument of LOR operataion expected to be INT");
                note(this->program[start], "In this evaluated constant expression");
                return Value(ValueType::NONE, "");
            }

            stack.push_back(Value(ValueType::INT, std::to_string(std::stol(over.text) || std::stol(top.text))));
        } break;
        case LAND: {
            Value top = stack.back();
            stack.pop_back();
            Value over = stack.back();
            stack.pop_back();

            if (top.type != ValueType::INT) {
                error(token, "Second argument of LAND operataion expected to be INT");
                note(this->program[start], "In this evaluated constant expression");
                return Value(ValueType::NONE, "");
            }
            if (over.type != ValueType::INT) {
                error(token, "First argument of LAND operataion expected to be INT");
                note(this->program[start], "In this evaluated constant expression");
                return Value(ValueType::NONE, "");
            }

            stack.push_back(Value(ValueType::INT, std::to_string(std::stol(over.text) && std::stol(top.text))));
        } break;
        case LNOT: {
            Value top = stack.back();
            stack.pop_back();

            if (top.type != ValueType::INT) {
                error(token, "Second argument of LAND operataion expected to be INT");
                note(this->program[start], "In this evaluated constant expression");
                return Value(ValueType::NONE, "");
            }

            stack.push_back(Value(ValueType::INT, std::to_string(!std::stol(top.text))));
        } break;
        case TRUE: {
            stack.push_back(Value(ValueType::INT, "1"));
        } break;
        case FALSE: {
            stack.push_back(Value(ValueType::INT, "0"));
        } break;
        case STRING:
        case STRING_DUMP:
        case STRING_PLUS:
        case DUMP:
        case INPUT:
        case BIND:
        case SAVE:
        case LOAD8:
        case LOAD16:
        case LOAD32:
        case LOAD64:
        case MAKECONSTEXPR:
        case ENDCONSTEXPR:
        case TERNARY:
        case MAKEPROC:
        case ENDPROC:
        case INVOKEPROC:
        case IF:
        case ENDIF:
        case WHILE:
        case DOWHILE:
        case ENDWHILE:
        case EQUAL:
        case NOTEQUAL:
        case LESS:
        case LESSEQUAL:
        case GREATER:
        case GREATEREQUAL:
        case TORAX:
        case TORBX:
        case TORCX:
        case TORDX:
        case TORSI:
        case TORDI:
        case TORBP:
        case TOR8:
        case TOR9:
        case TOR10:
        case SYSTEM_SYSCALL:
            error(token, fmt::format("{} operation is not supported in evaluated constant expression", TokenTypeString[token.type]));
            note(this->program[start], "In this evaluated constant expression");
            return Value(ValueType::NONE, "");
        case IDENT:
            error(token, "IDETs are not supported in evaluated constant expressions");
            note(this->program[start], "In this evaluated constant expression");
            return Value(ValueType::NONE, "");
        case UNDEFINED:
            std::cout << fmt::format("{} UwU ewwow: EndeyshentWabs made a fucky wucky!! A wittle fucko boingo!\n", token.pos.toString());
            note(this->program[start], "In this evaluated constant expression");
            std::exit(69);
            break;
        } // switfch
    } // foreach (token : exprProgram)

    if (stack.size() > 1) {
        error(this->program[start], "Unhandled data on top of the stack in this evaluated constant expression. Expected only 1 INT");
        return Value(ValueType::NONE, "");
    }

    if (stack.back().type != ValueType::INT) {
        error(this->program[start], "Expected INT as a result of evaluated constant expression");
        return Value(ValueType::NONE, "");
    }

    return Value(stack.back());
}

void Parser::error(Token token, std::string msg)
{
    fmt::print("{}: ERROR: {}.\n", token.pos.toString(), msg);
    this->hadError = true;
}

void Parser::note(Token token, std::string msg)
{
    fmt::print("{}: NOTE: {}.\n", token.pos.toString(), msg);
}
