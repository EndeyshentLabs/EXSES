#include <Lexer.hpp>

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

#include <utils.hpp>
#include <Token.hpp>
#include <Procedure.hpp>

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
        } else if (token.text == "_")  {
            token.type = DROP;
        } else if (token.text == "$")  {
            token.type = SWAP;
        } else if (token.text == "!")  {
            token.type = DUMP;
        } else if (token.text == "<-") {
            token.type = BIND;
        } else if (token.text == "<!") {
            token.type = SAVE;
        } else if (token.text == "^") {
            token.type = LOAD;
        } else if (token.text == ".?") {
            token.type = TERNARY;
        } else if (token.text == "'") {
            token.type = MAKEPROC;
        } else if (token.text == "\"") {
            token.type = ENDPROC;
        } else if (token.text == ":") {
            token.type = INVOKEPROC;
        } else if (token.text == "true") {
            token.type = PUSH;
            token.text = "1";
        } else if (token.text == "false") {
            token.type = PUSH;
            token.text = "0";
        } else {
            makeError(token, "Unexpected token `" + token.text + "`");
        }
    }
}

void Lexer::intrepret(bool insideOfProc, std::vector<Token> procBody)
{
    if (!insideOfProc && procBody.size() != 0) {
        std::cerr << "ERROR: procedure body outside of procedure";
        std::exit(1);
    }
    for (Token token : (insideOfProc ? procBody : this->program)) {
        if (!token.enabled && !insideOfProc) { continue; }
        switch (token.type) {
            case PUSH: {
                stack.push_back(stoi(token.text));
            } break;
            case DUP: {
                stack.push_back(stack.back());
            } break;
            case DROP: {
                if (stack.size() < 1)
                    makeError(token, "Not enough elements on the stack!");
                stack.pop_back();
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
            case SAVE: {
                if (stack.size() < 2)
                    makeError(token, "Not enough elements on the stack!");
                int value = stack.back();
                stack.pop_back();
                int name = stack.back();
                stack.pop_back();
                if (!storage.contains(name))
                    makeError(token, "There is no bind with that name!");
                storage[name] = value;
            } break;
            case LOAD: {
                int name = stack.back();
                stack.pop_back();
                if (!storage.contains(name))
                    makeError(token, "There is no bind with that name!");
                stack.push_back(storage[name]);
            } break;
            case TERNARY: {
                if (stack.size() < 3)
                    makeError(token, "Not enough elements on the stack!");
                int cond = stack.back();
                stack.pop_back();
                int alt = stack.back();
                stack.pop_back();
                int main = stack.back();
                stack.pop_back();

                if (cond) {
                    stack.push_back(main);
                } else {
                    stack.push_back(alt);
                }
            } break;
            case MAKEPROC: {
                if (stack.size() < 1)
                    makeError(token, "Not enough elements on the stack!");
                int name = stack.back();
                for (XesProcedure proc : procedureStorage) {
                    if (proc.name == name) {
                        makeError(token, "There is the procedure with the same name!");
                    }
                }
                stack.pop_back();
                std::vector<Token> body;
                bool hasEnd = false;
                for (auto& op : program) {
                    if (op.line < token.line || op.col < token.col || op.type == MAKEPROC) continue;
                    if (op.type == ENDPROC) {
                        op.enabled = false;
                        hasEnd = true;
                        break;
                    }
                    op.enabled = false;
                    body.push_back(op);
                }
                if (!hasEnd) {
                    makeError(token, "Unclosed procedure '" + std::to_string(name) + "'");
                }
                XesProcedure proc(token.line, token.col, name, body);
#               if defined(PROCEDURE_DEBUG)
                std::printf("%s:\tDEBUG: name: %d, body:\n", tokenLocation(token).c_str(), proc.name);
                for (auto op : proc.getBody()) {
                    std::printf("%s:\ttype: %s, text: '%s', enabled: %d\n", tokenLocation(op).c_str(), TokenTypeString[op.type].c_str(), op.text.c_str(), op.enabled);
                }
#               endif
                procedureStorage.push_back(proc);
            } break;
            case ENDPROC: {
                if (token.enabled) {
                    makeError(token, "Closing the procedure outside of any procedure!");
                }
            } break;
            case INVOKEPROC: {
                if (stack.size() < 1)
                    makeError(token, "Not enough elements on the stack!");
                int name = stack.back();
                stack.pop_back();
                bool found = false;
                for (XesProcedure proc : procedureStorage) {
                    if (proc.name == name) {
                        intrepret(true, proc.getBody());
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    makeError(token, "No such procedure '" + std::to_string(name) + "'");
                }
                // makeError(token, "Procedures is not implemented yet!");
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
            case DROP: {
                output.append("stack.pop()");
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
            case SAVE: {
                std::cerr << "ERROR: Binds(and Loads) is not implemented in python3 mode!\n";
                std::exit(1);
            } break;
            case LOAD: {
                std::cerr << "ERROR: Binds(and Loads) is not implemented in python3 mode!\n";
                std::exit(1);
            } break;
            case TERNARY: {
                std::cerr << "ERROR: Ternary operator is not implemented in python3 mode!\n";
                std::exit(1);
            }
            case MAKEPROC: {
                std::cerr << "ERROR: Procedures is not implemented in python3 mode!\n";
                std::exit(1);
            }
            case ENDPROC: {
                std::cerr << "ERROR: Procedures is not implemented in python3 mode!\n";
                std::exit(1);
            }
            case INVOKEPROC: {
                std::cerr << "ERROR: Procedures is not implemented in python3 mode!\n";
                std::exit(1);
            }
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

void Lexer::makeError(Token token, std::string text)
{
    std::cerr << tokenLocation(token) << ": ERROR: " << text << '\n';
    std::exit(1);
}

std::string Lexer::tokenLocation(Token token)
{
    return fileName + ":" + std::to_string(token.line + 1) + ":" + std::to_string(token.col + 1);
}
