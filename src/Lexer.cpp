#include <Lexer.hpp>

#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

#include <utils.hpp>
#include <Token.hpp>
#include <Procedure.hpp>

#define makeNote(tokenLike, text) \
    do { \
        std::cout << fileName + ":" + std::to_string((tokenLike).line + 1) + ":" + std::to_string((tokenLike).col + 1) << ": \033[32mNOTE\033[0m: " << text << '\n'; \
    } while (0)

std::vector<std::string> lines;

Lexer::Lexer(std::string fileName, Target target) : target(target), fileName(fileName)
{
    std::ifstream sourceFile;
    sourceFile.open(this->fileName, std::ios::in);
    if (sourceFile.is_open()) {
        std::string line;

        while (std::getline(sourceFile, line)) {
            source.append(line + '\n');
        }
        sourceFile.close();
    } else {
        std::cerr << "ERROR: Couldn't open a file '" << this->fileName << "'!\n";
    }

    this->run();
}

int stripByCol(std::string line, unsigned int col)
{
    while (col < line.length() && !std::isspace(line[col])) {
        col++;
    }
    return col;
}

int chopWord(std::string line, unsigned int col)
{
    while (col < line.length() && std::isspace(line[col])) {
        col++;
    }
    return col;
}

void Lexer::tokenize()
{
    lines = split(source, "\\n");
    unsigned int lineCount = 0;

    for (std::string line : lines) {
        unsigned int col = chopWord(line, 0);
        unsigned int colEnd = 0;

        if (line.find("#") != std::string::npos) {
            line = line.substr(0, line.find("#"));
        }

        while (col < line.length()) {
            colEnd = stripByCol(line, col);
            if (line.substr(col, colEnd).find(" ") != std::string::npos) { // TODO: make it better
                auto list = split(line.substr(col, colEnd), "\\s");
                std::string text = list.front();

                Token token(lineCount, col, makeType(text), text);

                if (token.type == UNDEFINED) {
                    makeError(token, "Unexpected token `" + token.text + "`");
                    printTokenLineInfo(token);
                    makeNote(token, "String cannot contain spaces!");
                    std::exit(1);
                }

                if (token.type == STRING) 
                    token.text = token.text.substr(1, text.length() - 2);

                this->program.push_back(token);
            } else {
                std::string text = line.substr(col, colEnd);

                Token token(lineCount, col, makeType(text), text);

                if (token.type == UNDEFINED) {
                    makeError(token, "Unexpected token `" + token.text + "`");
                    printTokenLineInfo(token);
                    makeNote(token, "String cannot contain spaces!");
                    std::exit(1);
                }

                if (token.type == STRING) 
                    token.text = token.text.substr(1, text.length() - 2);

                this->program.push_back(token);
            }
            col = chopWord(line, colEnd);
        }

        lineCount++;
    }
}

void Lexer::intrepret(bool inside, std::vector<Token> procBody)
{
    if (!inside && procBody.size() != 0) {
        std::cerr << "ERROR: EndeyshentLabs did something wrong! OMEGALUL\n";
        std::exit(1);
    }

    for (Token token : (inside ? procBody : this->program)) {
        if (!token.enabled && !inside) { continue; }
        switch (token.type) {
            case PUSH: {
                stack.push_back(token.text);
            } break;
            case STRING: {
                stack.push_back(token.text);
            } break;
            case DUP: {
                stack.push_back(stack.back());
            } break;
            case OVER: {
                if (stack.size() < 2) {
                    makeError(token, "Not enough elements on the stack!");
                    printTokenLineInfo(token);
                    std::exit(1);
                }

                std::string a = stack.back();
                stack.pop_back();
                std::string b = stack.back();

                stack.push_back(b);
                stack.push_back(a);
                stack.push_back(b);
            } break;
            case DROP: {
                if (stack.size() < 1) {
                    makeError(token, "Not enough elements on the stack! Expected 1, got " + std::to_string(stack.size()) + ".");
                    printTokenLineInfo(token);
                    std::exit(1);
                }

                stack.pop_back();
            } break;
            case SWAP: {
                if (stack.size() < 2) {
                    makeError(token, "Not enough elements on the stack! Expected 2, got " + std::to_string(stack.size()) + ".");
                    printTokenLineInfo(token);
                    std::exit(1);
                }

                std::string a = stack.back();
                stack.pop_back();
                std::string b = stack.back();
                stack.pop_back();

                stack.push_back(a);
                stack.push_back(b);
            } break;
            case PLUS: {
                if (stack.size() < 2) {
                    makeError(token, "Not enough elements on the stack! Expected 2, got " + std::to_string(stack.size()) + ".");
                    printTokenLineInfo(token);
                    std::exit(1);
                }

                long a = std::stol(stack.back());
                stack.pop_back();
                long b = std::stol(stack.back());
                stack.pop_back();

                stack.push_back(std::to_string(a + b));
            } break;
            case STRING_PLUS: {
                if (stack.size() < 2) {
                    makeError(token, "Not enough elements on the stack! Expected 2, got " + std::to_string(stack.size()) + ".");
                    printTokenLineInfo(token);
                    std::exit(1);
                }

                std::string a = stack.back();
                stack.pop_back();
                std::string b = stack.back();
                stack.pop_back();

                stack.push_back(a + b);
            } break;
            case MINUS: {
                if (stack.size() < 2) {
                    makeError(token, "Not enough elements on the stack! Expected 2, got " + std::to_string(stack.size()) + ".");
                    printTokenLineInfo(token);
                    std::exit(1);
                }

                long a = std::stol(stack.back());
                stack.pop_back();
                long b = std::stol(stack.back());
                stack.pop_back();

                stack.push_back(std::to_string(b - a));
            } break;
            case MULT: {
                if (stack.size() < 2) {
                    makeError(token, "Not enough elements on the stack! Expected 2, got " + std::to_string(stack.size()) + ".");
                    printTokenLineInfo(token);
                    std::exit(1);
                }

                long a = std::stol(stack.back());
                stack.pop_back();
                long b = std::stol(stack.back());
                stack.pop_back();

                stack.push_back(std::to_string(a * b));
            } break;
            case DIV: {
                if (stack.size() < 2) {
                    makeError(token, "Not enough elements on the stack! Expected 2, got " + std::to_string(stack.size()) + ".");
                    printTokenLineInfo(token);
                    std::exit(1);
                }

                long a = std::stol(stack.back());
                stack.pop_back();
                long b = std::stol(stack.back());
                stack.pop_back();

                stack.push_back(std::to_string(b / a));
            } break;
            case DUMP: {
                if (stack.size() < 1) {
                    makeError(token, "Not enough elements on the stack! Expected 1, got " + std::to_string(stack.size()) + ".");
                    printTokenLineInfo(token);
                    std::exit(1);
                }

                std::string a = stack.back();
                stack.pop_back();

                std::cout << a << '\n';
            } break;
            case BIND: {
                if (stack.size() < 2) {
                    makeError(token, "Not enough elements on the stack! Expected 2, got " + std::to_string(stack.size()) + ".");
                    printTokenLineInfo(token);
                    std::exit(1);
                }

                std::string value = stack.back();
                stack.pop_back();
                std::string name = stack.back();
                stack.pop_back();

                if (storage.contains(name)) {
                    makeError(token, "There is a bind with the name `" + name + "`!");
                    std::exit(1);
                }

                storage[name] = value;
            } break;
            case SAVE: {
                if (stack.size() < 2) {
                    makeError(token, "Not enough elements on the stack! Expected 2, got " + std::to_string(stack.size()) + ".");
                    printTokenLineInfo(token);
                    std::exit(1);
                }

                std::string value = stack.back();
                stack.pop_back();
                std::string name = stack.back();
                stack.pop_back();

                if (!storage.contains(name)) {
                    makeError(token, "There is no bind with the name `" + name + "`!");
                    std::exit(1);
                }

                storage[name] = value;
            } break;
            case LOAD: {
                std::string name = stack.back();
                stack.pop_back();

                if (!storage.contains(name)) {
                    makeError(token, "There is no bind with that name!");
                    std::exit(1);
                }

                stack.push_back(storage[name]);
            } break;
            case TERNARY: {
                if (stack.size() < 3) {
                    makeError(token, "Not enough elements on the stack! Expected 3, got " + std::to_string(stack.size()) + ".");
                    printTokenLineInfo(token);
                    std::exit(1);
                }

                long cond = std::stol(stack.back());
                stack.pop_back();
                std::string alt = stack.back();
                stack.pop_back();
                std::string main = stack.back();
                stack.pop_back();

                if (cond) {
                    stack.push_back(main);
                } else {
                    stack.push_back(alt);
                }
            } break;
            case MAKEPROC: {
                if (inside) {
                    makeError(token, "Possible procedure inside of another procedure");
                    printTokenLineInfo(token);
                    std::exit(1);
                }

                if (stack.size() < 1) {
                    makeError(token, "Not enough elements on the stack! Expected name of the procedure.");
                    printTokenLineInfo(token);
                    std::exit(1);
                }

                std::string name = stack.back();

                for (Procedure proc : procedureStorage) {
                    if (proc.name == name) {
                        makeError(token, "There is the procedure with the name `" + proc.name + "`!");
                        printTokenLineInfo(token);

                        makeNote(proc, "Defined here.");
                        printTokenLineInfo(proc);
                        std::exit(1);
                    }
                }

                stack.pop_back();

                std::vector<Token> body;
                bool hasEnd = false;

                for (Token& op : program) {
                    if (op.line < token.line || (op.line == token.line && op.col < token.col) || op.type == MAKEPROC) continue;
                    if (op.type == ENDPROC && op.enabled) {
                        op.enabled = false;
                        hasEnd = true;
                        break;
                    }
                    op.enabled = false;
                    body.push_back(op);
                }

                if (!hasEnd) {
                    makeError(token, "Unclosed procedure '" + name + "'");
                    printTokenLineInfo(token);
                    std::exit(1);
                }

                Procedure proc(token.line, token.col, name, body);
                procedureStorage.push_back(proc);
            } break;
            case ENDPROC: {
                if (token.enabled) {
                    makeError(token, "Closing the procedure outside of any procedure!");
                    printTokenLineInfo(token);
                    std::exit(1);
                }
            } break;
            case INVOKEPROC: {
                if (stack.size() < 1) {
                    makeError(token, "Not enough elements on the stack! Expected name of the procedure to invoke.");
                    printTokenLineInfo(token);
                    std::exit(1);
                }

                std::string name = stack.back();
                stack.pop_back();

                bool found = false;

                for (Procedure proc : procedureStorage) {
                    if (proc.name == name) {
                        intrepret(true, proc.getBody());
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    makeError(token, "No such procedure '" + name + "'");
                    std::exit(1);
                }
            } break;
            case IF: {
                if (inside) break;
                if (stack.size() < 1) {
                    makeError(token, "Not enough elements on the stack! Expected condition.");
                    printTokenLineInfo(token);
                    std::exit(1);
                }

                long cond = std::stol(stack.back());
                stack.pop_back();

                std::vector<Token> body;
                bool hasEnd = false;

                for (auto& op : program) {
                    if (op.line < token.line || (op.line == token.line && op.col < token.col) || op.type == IF) continue;
                    if (op.type == ENDIF) {
                        hasEnd = true;
                        op.enabled = false;
                        break;
                    }
                    op.enabled = false;
                    body.push_back(op);
                }

                if (!hasEnd) {
                    makeError(token, "Unclosed IF");
                    printTokenLineInfo(token);
                    std::exit(1);
                }

                if (cond) {
                    intrepret(true, body);
                }
            } break;
            case ENDIF: {
                if (token.enabled) {
                    makeError(token, "Closing IF statement outside of IF statement");
                    printTokenLineInfo(token);
                    std::exit(1);
                }
            } break;
            case EQUAL: {
                if (stack.size() < 2) {
                    makeError(token, "Not enough elements on the stack!");
                    printTokenLineInfo(token);
                    std::exit(1);
                }

                std::string a = stack.back();
                stack.pop_back();
                std::string b = stack.back();
                stack.pop_back();

                stack.push_back(std::to_string(b == a));
            } break;
            case NOTEQUAL: {
                if (stack.size() < 2) {
                    makeError(token, "Not enough elements on the stack!");
                    printTokenLineInfo(token);
                    std::exit(1);
                }

                std::string a = stack.back();
                stack.pop_back();
                std::string b = stack.back();
                stack.pop_back();

                stack.push_back(std::to_string(b != a));
            } break;
            case LESS: {
                if (stack.size() < 2) {
                    makeError(token, "Not enough elements on the stack!");
                    printTokenLineInfo(token);
                    std::exit(1);
                }
                long a = std::stol(stack.back());
                stack.pop_back();
                long b = std::stol(stack.back());
                stack.pop_back();

                stack.push_back(std::to_string(b < a));
            } break;
            case LESSEQUAL: {
                if (stack.size() < 2) {
                    makeError(token, "Not enough elements on the stack!");
                    printTokenLineInfo(token);
                    std::exit(1);
                }

                long a = std::stol(stack.back());
                stack.pop_back();
                long b = std::stol(stack.back());
                stack.pop_back();

                stack.push_back(std::to_string(b <= a));
            } break;
            case GREATER: {
                if (stack.size() < 2) {
                    makeError(token, "Not enough elements on the stack!");
                    printTokenLineInfo(token);
                    std::exit(1);
                }

                long a = std::stol(stack.back());
                stack.pop_back();
                long b = std::stol(stack.back());
                stack.pop_back();

                stack.push_back(std::to_string(b > a));
            } break;
            case GREATEREQUAL: {
                if (stack.size() < 2) {
                    makeError(token, "Not enough elements on the stack!");
                    printTokenLineInfo(token);
                    std::exit(1);
                }

                long a = std::stol(stack.back());
                stack.pop_back();
                long b = std::stol(stack.back());
                stack.pop_back();

                stack.push_back(std::to_string(b >= a));
            } break;
            case LOR: {
                if (stack.size() < 2) {
                    makeError(token, "Not enough elements on the stack!");
                    printTokenLineInfo(token);
                    std::exit(1);
                }

                long a = std::stol(stack.back());
                stack.pop_back();
                long b = std::stol(stack.back());
                stack.pop_back();

                stack.push_back(std::to_string(b || a));
            } break;
            case LAND: {
                if (stack.size() < 2) {
                    makeError(token, "Not enough elements on the stack!");
                    printTokenLineInfo(token);
                    std::exit(1);
                }

                long a = std::stol(stack.back());
                stack.pop_back();
                long b = std::stol(stack.back());
                stack.pop_back();

                stack.push_back(std::to_string(b && a));
            } break;
            case LNOT: {
                if (stack.size() < 1) {
                    makeError(token, "Not enough elements on the stack!");
                    printTokenLineInfo(token);
                    std::exit(1);
                }

                long a = std::stol(stack.back());
                stack.pop_back();

                stack.push_back(std::to_string(!a));
            } break;
            case TRUE: {
                stack.push_back(std::to_string(1));
            } break;
            case FALSE: {
                stack.push_back(std::to_string(0));
            } break;
            // START STDLIB
            case STDLIB_MATH_SQRT: {
                if (stack.size() < 1) {
                    makeError(token, "Not enough elements on the stack!");
                    printTokenLineInfo(token);
                    std::exit(1);
                }

                makeNote(token, "Warning: result of sqrt operation will be an integer!");
                long a = std::stol(stack.back());
                stack.pop_back();

                stack.push_back(std::to_string(std::sqrt(a)));
            } break;
            case STDLIB_MATH_COS: {
                if (stack.size() < 1) {
                    makeError(token, "Not enough elements on the stack!");
                    printTokenLineInfo(token);
                    std::exit(1);
                }

                makeNote(token, "Warning: result of cos operation will be an integer!");
                long a = std::stol(stack.back());
                stack.pop_back();

                stack.push_back(std::to_string(std::cos(a)));
            } break;
            case STDLIB_MATH_SIN: {
                if (stack.size() < 1) {
                    makeError(token, "Not enough elements on the stack!");
                    printTokenLineInfo(token);
                    std::exit(1);
                }

                makeNote(token, "Warning: result of sin operation will be an integer!");
                long a = std::stol(stack.back());
                stack.pop_back();

                stack.push_back(std::to_string(std::sin(a)));
            } break;
            case UNDEFINED: {
                std::cerr << "ERROR: UNREACHABLE\n";
                std::exit(1);
            } break;
        }
    }
}

void Lexer::run()
{
    this->tokenize();
#   if !defined(NDEBUG)
    std::cout << "Program:\n";
    for (Token token : program) {
        std::printf("%s: type: %s, text: `%s`, enabled: %s\n", tokenLocation(token).c_str(), TokenTypeString[token.type].c_str(), token.text.c_str(), (token.enabled) ? "true" : "false");
    }
#   endif
    if (this->target == EXSI) {
        this->intrepret();
    } else {
        std::cerr << "Only EXSI target is supported right now!\n";
        std::exit(1);
    }
}

TokenType Lexer::makeType(std::string text)
{
    if (isInteger(text)) {
        return(PUSH);
    } else if (text == "[+]")  {
        return(STRING_PLUS);
    } else if (std::regex_match(text, std::regex("\\[.*\\]"))) {
        return(STRING);
    } else if (text == "+")  {
        return(PLUS);
    } else if (text == "-")  {
        return(MINUS);
    } else if (text == "*")  {
        return(MULT);
    } else if (text == "/")  {
        return(DIV);
    } else if (text == "&")  {
        return(DUP);
    } else if (text == "$&")  {
        return(OVER);
    } else if (text == "_")  {
        return(DROP);
    } else if (text == "$")  {
        return(SWAP);
    } else if (text == "!")  {
        return(DUMP);
    } else if (text == "<-") {
        return(BIND);
    } else if (text == "<!") {
        return(SAVE);
    } else if (text == "^") {
        return(LOAD);
    } else if (text == ".?") {
        return(TERNARY);
    } else if (text == "'") {
        return(MAKEPROC);
    } else if (text == "\"") {
        return(ENDPROC);
    } else if (text == ":") {
        return(INVOKEPROC);
    } else if (text == "(") {
        return(IF);
    } else if (text == ")") {
        return(ENDIF);
    } else if (text == "=") {
        return(EQUAL);
    } else if (text == "<>") {
        return(NOTEQUAL);
    } else if (text == "<") {
        return(LESS);
    } else if (text == "<=") {
        return(LESSEQUAL);
    } else if (text == ">") {
        return(GREATER);
    } else if (text == ">=") {
        return(GREATEREQUAL);
    } else if (text == "||") {
        return(LOR);
    } else if (text == "&&") {
        return(LAND);
    } else if (text == "!!") {
        return(LNOT);
    } else if (text == "true") {
        return(TRUE);
    } else if (text == "false") {
        return(FALSE);
    // STDLIB
    } else if (text == "Math(sqrt)") {
        return(STDLIB_MATH_SQRT);
    } else if (text == "Math(cos)") {
        return(STDLIB_MATH_COS);
    } else if (text == "Math(sin)") {
        return(STDLIB_MATH_SIN);
    }

    return UNDEFINED;
}

void Lexer::makeError(Token token, std::string text)
{
    std::cerr << tokenLocation(token) << ": \033[31mERROR\033[0m: " << text << '\n';
}

std::string Lexer::tokenLocation(Token token)
{
    return fileName + ":" + std::to_string(token.line + 1) + ":" + std::to_string(token.col + 1);
}

template<typename T> // This trick is needed for compatibility with both Token and Procedure classes
void printTokenLineInfo(T token)
{
    std::printf("%d | %s\n", token.line+1, lines[token.line].c_str());
    for (unsigned int i = 0; i < std::to_string(token.line).length(); i++) {
        std::cout << " ";
    }
    std::cout << " | \033[31m";
    for (unsigned int i = 0; i < token.col; i++) {
        std::cout << " ";
    }
    std::cout << "^\033[0m\n";
}
