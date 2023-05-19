#include <Lexer.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

#include <utils.hpp>
#include <Token.hpp>

Lexer::Lexer(std::string fileName, std::string source, Target target) : target(target), source(source), fileName(fileName)
{
}

int stripByCol(std::string line, int col)
{
    while (col < line.length() && !std::isspace(line[col])) {
        col++;
    }
    return col;
}

int chopWord(std::string line, int col)
{
    while (col < line.length() && std::isspace(line[col])) {
        col++;
    }
    return col;
}

void Lexer::tokenize()
{
    std::vector<std::string> lines = split(source, "\\n");
    unsigned int lineCount = 0;

    for (std::string line : lines) {
        int col = chopWord(line, 0);
        int colEnd = 0;

        if (line.find("#") != std::string::npos) {
            line = line.substr(0, line.find("#"));
        }

        while (col < line.length()) {
            colEnd = stripByCol(line, col);
            if (line.substr(col, colEnd).find(" ") != std::string::npos) {
                auto list = split(line.substr(col, colEnd), "\\s");
                this->program.push_back(Token{ static_cast<int>(lineCount), col, UNDEFINED, list.front() });
            } else {
                this->program.push_back(Token{ static_cast<int>(lineCount), col, UNDEFINED, line.substr(col, colEnd) });
            }
            col = chopWord(line, colEnd);
        }

        lineCount++;
    }

    for (auto& token : program) {
        if (isInteger(token.text)) {
            token.type = PUSH;
        } else if (token.text == "+")  {
            token.type = PLUS;
        } else if (token.text == "-")  {
            token.type = MINUS;
        } else if (token.text == "*")  {
            token.type = MULT;
        } else if (token.text == "/")  {
            token.type = DIV;
        } else if (token.text == "&")  {
            token.type = DUP;
        } else if (token.text == "$")  {
            token.type = SWAP;
        } else if (token.text == "!")  {
            token.type = DUMP;
        } else if (token.text == "<-") {
            token.type = BIND;
        } else if (token.text == "^") {
            token.type = LOAD;
        } else {
            makeError(token, "Unexpected token `" + token.text + "`");
        }
    }
}

void Lexer::intrepret()
{
    for (Token token : this->program) {
        switch (token.type) {
            case PUSH: {
                stack.push_back(stoi(token.text));
            } break;
            case DUP: {
                stack.push_back(stack.back());
            } break;
            case SWAP: {
                if (stack.size() < 2)
                    makeError(token, "Not enough elements on the stack!");
                int a = stack.back();
                stack.pop_back();
                int b = stack.back();
                stack.pop_back();
                stack.push_back(a);
                stack.push_back(b);
            } break;
            case PLUS: {
                if (stack.size() < 2)
                    makeError(token, "Not enough elements on the stack!");
                int a = stack.back();
                stack.pop_back();
                int b = stack.back();
                stack.pop_back();
                stack.push_back(a + b);
            } break;
            case MINUS: {
                if (stack.size() < 2)
                    makeError(token, "Not enough elements on the stack!");
                int a = stack.back();
                stack.pop_back();
                int b = stack.back();
                stack.pop_back();
                stack.push_back(b - a);
            } break;
            case MULT: {
                if (stack.size() < 2)
                    makeError(token, "Not enough elements on the stack!");
                int a = stack.back();
                stack.pop_back();
                int b = stack.back();
                stack.pop_back();
                stack.push_back(a * b);
            } break;
            case DIV: {
                if (stack.size() < 2)
                    makeError(token, "Not enough elements on the stack!");
                int a = stack.back();
                stack.pop_back();
                int b = stack.back();
                stack.pop_back();
                stack.push_back(b / a);
            } break;
            case DUMP: {
                if (stack.size() < 1)
                    makeError(token, "Not enough elements on the stack!");
                int a = stack.back();
                stack.pop_back();
                std::cout << a << '\n';
            } break;
            case BIND: {
                if (stack.size() < 2)
                    makeError(token, "Not enough elements on the stack!");
                int value = stack.back();
                stack.pop_back();
                int name = stack.back();
                stack.pop_back();
                if (storage.contains(name))
                    makeError(token, "There is a bind with the same name!");
                storage[name] = value;
            } break;
            case LOAD: {
                int name = stack.back();
                stack.pop_back();
                if (!storage.contains(name))
                    makeError(token, "There is no bind with that name!");
                stack.push_back(storage[name]);
            } break;
            case UNDEFINED: {
                std::cerr << "ERROR: UNREACHABLE IS REACHED!!!1!\n";
                std::exit(1);
            } break;
        }
    }
}

void Lexer::compileToPython3()
{
    std::string output;
    output.append("#!/usr/bin/env python3\nstack = []\n");

    for (auto token : program) {
        switch (token.type) {
            case PUSH: {
                output.append("stack.append(" + token.text + ")\n");
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
                output.append("stack.append(stack.pop(0) / stack.pop())\n");
            } break;
            case DUMP: {
                output.append("print(stack.pop())\n");
            } break;
            case BIND: {
                std::cerr << "ERROR: Binds is not implemented in python3 mode!\n";
                std::exit(1);
            } break;
            case LOAD: {
                std::cerr << "ERROR: Binds(and Loads) is not implemented in python3 mode!\n";
                std::exit(1);
            } break;
            case UNDEFINED: {
                std::cerr << "ERROR: UNREACHABLE IS REACHED!!!1!\n";
                std::exit(1);
            } break;
        }
    }

    std::cout << output;
}

void Lexer::run()
{
    this->tokenize();
    if (this->target == EXSI) {
        this->intrepret();
    } else if (this->target == PYTHON3) {
        std::cerr << "WARNING: Python3 mode is not fininised yet! It's not recomended to use this mode right now!\n";
        this->compileToPython3();
    }
}

void Lexer::nextToken()
{
    this->trimLeft();
    while (this->isNotEmpty()) {
        std::string s = source.substr(this->cur);
        if (!s.starts_with("#") && !s.starts_with("//")) { break; }
        this->dropLine();
        this->trimLeft();
    }
}

void Lexer::chopChar()
{
    if (this->isNotEmpty()) {
        char x = this->source[this->cur];
        this->cur++;
        if (x == '\n') {
            this->bol = this->cur;
            this->row++;
        }
    }
}

void Lexer::trimLeft()
{
    while (this->isNotEmpty() && std::isspace(this->source[this->cur])) {
        this->chopChar();
    }
}

void Lexer::dropLine()
{
    while (this->isNotEmpty() && this->source[this->cur] != '\n') {
        this->chopChar();
    }

    if (this->isNotEmpty()) {
        this->chopChar();
    }
}

void Lexer::makeError(Token token, std::string text)
{
    std::cerr << fileName << ":" << token.line + 1 << ":" << token.col + 1 << ": ERROR: " << text << '\n';
    std::exit(1);
}
