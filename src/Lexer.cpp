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

    for (auto& el : program) {
        if (isInteger(el.text)) {
            el.type = PUSH;
        } else if (el.text == "+")  {
            el.type = PLUS;
        } else if (el.text == "-")  {
            el.type = MINUS;
        } else if (el.text == "*")  {
            el.type = MULT;
        } else if (el.text == "/")  {
            el.type = DIV;
        } else if (el.text == "&")  {
            el.type = DUP;
        } else if (el.text == "$")  {
            el.type = SWAP;
        } else if (el.text == "!")  {
            el.type = DUMP;
        } else {
            std::cerr << fileName << ":" << el.line + 1 << ":" << el.col + 1 << ": ERROR: Unexpected token `" << el.text << "`\n";
            std::exit(1);
        }
    }
}

void Lexer::parse()
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
                int a = stack.back();
                stack.pop_back();
                int b = stack.back();
                stack.pop_back();
                stack.push_back(a);
                stack.push_back(b);
            } break;
            case PLUS: {
                int a = stack.back();
                stack.pop_back();
                int b = stack.back();
                stack.pop_back();
                stack.push_back(a + b);
            } break;
            case MINUS: {
                int a = stack.back();
                stack.pop_back();
                int b = stack.back();
                stack.pop_back();
                stack.push_back(b - a);
            } break;
            case MULT: {
                int a = stack.back();
                stack.pop_back();
                int b = stack.back();
                stack.pop_back();
                stack.push_back(a * b);
            } break;
            case DIV: {
                int a = stack.back();
                stack.pop_back();
                int b = stack.back();
                stack.pop_back();
                stack.push_back(b / a);
            } break;
            case DUMP: {
                int a = stack.back();
                stack.pop_back();
                std::cout << a << '\n';
            } break;
            default: { std::cerr << "ERROR: UNREACHABLE IS REACHED!!!1!\n"; } break;
        }
    }
}

void Lexer::intrepret()
{
    this->tokenize();
    if (this->target == EXSI) {
        this->parse();
    } else {
        std::cerr << "ERROR: Only exsi target is supported!";
    }
}

void Lexer::nextToken()
{
    this->trimLeft();
    while (this->isNotEmpty()) {
        std::string s = source.substr(this->cur);
        if (!s.starts_with(";") && !s.starts_with("//")) { break; }
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
