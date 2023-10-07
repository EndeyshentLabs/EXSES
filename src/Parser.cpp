#include <Parser.hpp>

#include <Lexer.hpp>
#include <Token.hpp>

#include <format>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <vector>

#define CMD(command)                                   \
    do {                                               \
        std::cout << "COMMAND: " << (command) << '\n'; \
        std::system((command).c_str());                \
    } while (0)

enum class BindingSize {
    BYTE = 8,
    WORD = 16,
    DWORD = 32,
    QWORD = 64
};

#define STR_(x) #x
#define STR(x) STR_(x)

std::map<std::string, unsigned int> procs;
std::map<std::string, BindingSize> bindings;

#define LOAD_COMMON()                                                                                       \
    if (this->program.size() - ip - 1 == 0)                                                                 \
        error(token, "Expected binding name as IDENT but got nothing");                                     \
    Token& name = this->program[ip - 1];                                                                    \
    if (name.type != IDENT)                                                                                 \
        error(token, std::format("Expected binding name as IDENT but got {}", TokenTypeString[name.type])); \
    name.processedByParser = true;                                                                          \
    if (!bindings.contains(name.value.text)) {                                                              \
        error(token, std::format("Binding '{}' is not defined", name.value.text));                          \
    }                                                                                                       \
    (void)NULL

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
                       "extern puts\n"
                       "global _start\n"
                       "section .text\n"
                       "_start:\n"
                       "    mov rax, saved_rsp_end\n"
                       "    mov [saved_rsp], rax\n");

    std::cout << "INFO: Generating " << outputFilepath << '\n';

    std::vector<std::string> stringStorage;

    for (; ip < this->program.size(); ip++) {
        Token token = this->program[ip];

        switch (token.type) {
        case PUSH: {
            output.append(std::format("addr_{}: ;; {}: PUSH {}\n", ip, token.pos.toString(), std::stoi(token.value.text)));
            output.append(std::format("    mov r11, {}\n", token.value.text));
            output.append("    push r11\n");
        } break;
        case STRING: {
            output.append(std::format("addr_{}: ;; {}: STRING {}\n", ip, token.pos.toString(), token.value.text));
            output.append(std::format("    push str_{}\n", stringStorage.size()));
            stringStorage.push_back(token.value.text);
        } break;
        case STRING_DUMP: {
            output.append(std::format("addr_{}: ;; {}: STRING_DUMP\n", ip, token.pos.toString()));
            output.append("    pop rdi\n");
            output.append("    xor eax, eax\n");
            output.append("    call puts\n");
        } break;
        case STRING_PLUS: {
            output.append(std::format("addr_{}: ;; {}: STRING_PLUS\n", ip, token.pos.toString()));
            error(token, "String-plus operation is not implemented in nasm-linux-x86_64!");
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
            output.append("    pop r11\n");
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
            error(token, "User input is not implemented in NASM_LINUX_X86_64 target!");
        } break;
        case BIND: {
            output.append(std::format("addr_{}: ;; {}: BIND\n", ip, token.pos.toString()));

            if (this->program.size() - ip - 1 == 0)
                error(token, "Expected binding size as IDENT but got nothing");

            Token& size = this->program[ip - 1];

            if (size.type != IDENT)
                error(token, std::format("Expected size name as IDENT but got {}", TokenTypeString[size.type]));
            size.processedByParser = true;

            if (this->program.size() - ip - 2 == 0)
                error(token, "Expected binding name as IDENT but got nothing");

            Token& name = this->program[ip - 2];

            if (name.type != IDENT)
                error(token, std::format("Expected binding name as IDENT but got {}", TokenTypeString[name.type]));
            name.processedByParser = true;

            BindingSize bindingSize;

            if (size.value.text == "byte") {
                bindingSize = BindingSize::BYTE;
            } else if (size.value.text == "word") {
                bindingSize = BindingSize::WORD;
            } else if (size.value.text == "dword") {
                bindingSize = BindingSize::DWORD;
            } else if (size.value.text == "qword") {
                bindingSize = BindingSize::QWORD;
            } else {
                error(token, std::format("Unknown binding size '{}'", size.value.text));
            }

            bindings.insert_or_assign(name.value.text, bindingSize);
        } break;
        case SAVE: {
            output.append(std::format("addr_{}: ;; {}: SAVE\n", ip, token.pos.toString()));

            if (this->program.size() - ip - 1 == 0)
                error(token, "Expected binding name as IDENT but got nothing");

            Token& name = this->program[ip - 1];

            if (name.type != IDENT)
                error(token, std::format("Expected binding name as IDENT but got {}", TokenTypeString[name.type]));
            name.processedByParser = true;

            if (!bindings.contains(name.value.text)) {
                error(token, std::format("Binding '{}' is not defined", name.value.text));
            }

            std::string cast;

            output.append("    pop r11\n");
            output.append(std::format("    mov [{}], r11\n", name.value.text));
        } break;
        case LOAD: {
            output.append(std::format("addr_{}: ;; {}: LOAD\n", ip, token.pos.toString()));

            LOAD_COMMON();

            output.append(std::format("    mov r11, {}\n", name.value.text));
            output.append("    push r11\n");
        } break;
        case LOAD8: {
            output.append(std::format("addr_{}: ;; {}: LOAD\n", ip, token.pos.toString()));

            LOAD_COMMON();

            output.append(std::format("    mov r11, {}\n", name.value.text));
            output.append("    xor r12, r12\n");
            output.append("    mov r12b, [r11]\n");
            output.append("    push r12\n");
        } break;
        case LOAD16: {
            output.append(std::format("addr_{}: ;; {}: LOAD\n", ip, token.pos.toString()));

            LOAD_COMMON();

            output.append(std::format("    mov r11, {}\n", name.value.text));
            output.append("    xor r12, r12\n");
            output.append("    mov r12w, [r11]\n");
            output.append("    push r12\n");
        } break;
        case LOAD32: {
            output.append(std::format("addr_{}: ;; {}: LOAD\n", ip, token.pos.toString()));

            LOAD_COMMON();

            output.append(std::format("    mov r11, {}\n", name.value.text));
            output.append("    xor r12, r12\n");
            output.append("    mov r12d, [r11]\n");
            output.append("    push r12\n");
        } break;
        case LOAD64: {
            output.append(std::format("addr_{}: ;; {}: LOAD\n", ip, token.pos.toString()));

            LOAD_COMMON();

            output.append(std::format("    mov r11, {}\n", name.value.text));
            output.append("    xor r12, r12\n");
            output.append("    mov r12, [r11]\n");
            output.append("    push r12\n");
        } break;
        case TERNARY: {
            output.append(std::format("addr_{}: ;; {}: TERNARY\n", ip, token.pos.toString()));
            error(token, "Ternary operator is not implemented in NASM_LINUX_X86_64 target!");
        } break;
        case MAKEPROC: {
            if (this->program.size() - ip - 1 == 0)
                error(token, "Expected function name as IDENT but got nothing");

            Token& before = this->program[ip - 1];

            if (before.type != IDENT)
                error(token, std::format("Expected function name as IDENT but got {}", TokenTypeString[before.type]));
            before.processedByParser = true;

            procs.insert_or_assign(before.value.text, ip);

            output.append(";; PRE-MAKEPROC\n");
            output.append(std::format("    jmp addr_{}\n", token.pairIp));
            output.append(std::format("addr_{}: ;; {}: MAKEPROC (to {})\n", ip, token.pos.toString(), token.pairIp));
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
            output.append(std::format("addr_{}: ;; {}: ENDPROC\n", ip, token.pos.toString()));
        } break;
        case INVOKEPROC: {
            if (this->program.size() - ip - 1 == 0)
                error(token, "Expected function name as IDENT but got nothing");

            Token& before = this->program[ip - 1];

            if (before.type != IDENT)
                error(token, std::format("Expected function name as IDENT but got {}", TokenTypeString[before.type]));
            before.processedByParser = true;

            output.append(std::format("addr_{}: ;; {}: INVOKEPROC\n", ip, token.pos.toString()));
            output.append("    mov rax, rsp\n");
            output.append("    mov rsp, [saved_rsp]\n");
            output.append(std::format("    call addr_{}\n", procs[before.value.text]));
            output.append("    mov [saved_rsp], rsp\n");
            output.append("    mov rsp, rax\n");
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
        case TORAX: {
            output.append(std::format("addr_{}: ;; {}: TORAX\n", ip, token.pos.toString()));
            output.append("    pop rax\n");
        } break;
        case TORBX: {
            output.append(std::format("addr_{}: ;; {}: TORBX\n", ip, token.pos.toString()));
            output.append("    pop rbx\n");
        } break;
        case TORCX: {
            output.append(std::format("addr_{}: ;; {}: TORCX\n", ip, token.pos.toString()));
            output.append("    pop rcx\n");
        } break;
        case TORDX: {
            output.append(std::format("addr_{}: ;; {}: TORDX\n", ip, token.pos.toString()));
            output.append("    pop rdx\n");
        } break;
        case TORSI: {
            output.append(std::format("addr_{}: ;; {}: TORSI\n", ip, token.pos.toString()));
            output.append("    pop rsi\n");
        } break;
        case TORDI: {
            output.append(std::format("addr_{}: ;; {}: TORDI\n", ip, token.pos.toString()));
            output.append("    pop rdi\n");
        } break;
        case TORBP: {
            output.append(std::format("addr_{}: ;; {}: TORBP\n", ip, token.pos.toString()));
            output.append("    pop rbp\n");
        } break;
        case TOR8: {
            output.append(std::format("addr_{}: ;; {}: TOR8\n", ip, token.pos.toString()));
            output.append("    pop r8\n");
        } break;
        case TOR9: {
            output.append(std::format("addr_{}: ;; {}: TOR9\n", ip, token.pos.toString()));
            output.append("    pop r9\n");
        } break;
        case TOR10: {
            output.append(std::format("addr_{}: ;; {}: TOR10\n", ip, token.pos.toString()));
            output.append("    pop r10\n");
        } break;
        case SYSTEM_SYSCALL: {
            output.append(std::format("addr_{}: ;; {}: SYSCALL\n", ip, token.pos.toString()));
            output.append("    syscall\n");
        } break;
        case IDENT: {
            // TODO: Learn how to set `processedByParser` by reference
            if (!token.processedByParser) {
                // error(token, std::format("Unexpected IDENT '{}'", token.value.text));
            }
        } break;
        case UNDEFINED:
        case TRUE:
        case FALSE: {
            std::cout << std::format("{}:{} UwU ewwow: EndeyshentWabs made a fucky wucky!! A wittle fucko boingo!\n", this->inputFileName, token.pos.toString());
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
        // Inspired by https://gitlab.com/tsoding/porth
        output.append("    saved_rsp: resq 1\n");
        output.append("    saved_rsp_offset: resb 65536\n");
        output.append("    saved_rsp_end:\n");

        for (auto const& [name, size] : bindings) {
            char mnemonic;
            switch (size) {
            case BindingSize::BYTE:
                mnemonic = 'b';
                break;
            case BindingSize::WORD:
                mnemonic = 'w';
                break;
            case BindingSize::DWORD:
                mnemonic = 'd';
                break;
            case BindingSize::QWORD:
                mnemonic = 'q';
                break;
            }

            output.append(std::format("    {}: res{} 1\n", name, mnemonic));
        }
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

    // TODO: Should stop compilation if failed
    CMD(std::format("nasm -g -felf64 {}", outputFilepath));
    CMD(std::format("ld -dynamic-linker /lib64/ld-linux-x86-64.so.2 -o {} {} -lc", std::regex_replace(outputFilepath, std::regex("\\.asm$"), ""), std::regex_replace(outputFilepath, std::regex("\\.asm$"), ".o")));
}

void Parser::error(Token token, std::string msg)
{
    std::cout << std::format("{}:{}: ERROR: {}.\n", this->inputFileName, token.pos.toString(), msg);
    std::exit(1);
}
