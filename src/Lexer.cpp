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

void Lexer::tokenize()
{
    std::vector<std::string> words = split(source, "\\s");

    for (const auto& word : words) {
        if (isInteger(word)) {
            this->program.push_back(Token(PUSH, std::stoi(word)));
        } else if (word == "+") {
            this->program.push_back(Token(PLUS));
        } else if (word == "-") {
            this->program.push_back(Token(MINUS));
        } else if (word == "*") {
            this->program.push_back(Token(MULT));
        } else if (word == "/") {
            this->program.push_back(Token(DIV));
        } else if (word == "&") {
            this->program.push_back(Token(DUP));
        } else if (word == "$") {
            this->program.push_back(Token(SWAP));
        } else if (word == "!") {
            this->program.push_back(Token(DUMP));
        } else {
            // Exrtract location in getter function
            std::cerr << this->fileName << ":" << this->row << ":" << this->cur - this->bol << ": ERROR: Unknown word `" << word << "`\n";
            std::exit(1);
        }
    }
}

void Lexer::parse()
{
    for (Token token : this->program) {
        switch (token.type) {
            case PUSH: {
                stack.push_back(token.getValue());
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
                stack.push_back(a / b);
            } break;
            case DUMP: {
                int a = stack.back();
                stack.pop_back();
                std::cout << a << '\n';
            } break;
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
    while (this->isNotEmpty() && isspace(this->source[this->cur])) {
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

Location Lexer::loc()
{
    return Location(this->fileName, this->row, this->cur - this->bol);
}

Token Lexer::makeNumber()
{
    int start = this->cur;

    while (this->isNotEmpty() && std::isdigit(this->source[this->cur])) {
        this->chopChar();
    }

    int value = std::stoi(this->source.substr(start, this->cur - start));

    return Token(PUSH, value, this->loc());
}
