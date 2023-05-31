#include <Lexer.hpp>

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
            if (line.substr(col, colEnd).find(" ") != std::string::npos) {
                auto list = split(line.substr(col, colEnd), "\\s");
                this->program.push_back(Token{ lineCount, col, UNDEFINED, list.front() });
            } else {
                this->program.push_back(Token{ lineCount, col, UNDEFINED, line.substr(col, colEnd) });
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
        } else if (token.text == "$&")  {
            token.type = OVER;
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
        } else if (token.text == "(") {
            token.type = IF;
        } else if (token.text == ")") {
            token.type = ENDIF;
        } else if (token.text == "=") {
            token.type = EQUAL;
        } else if (token.text == "<>") {
            token.type = NOTEQUAL;
        } else if (token.text == "<") {
            token.type = LESS;
        } else if (token.text == "<=") {
            token.type = LESSEQUAL;
        } else if (token.text == ">") {
            token.type = GREATER;
        } else if (token.text == ">=") {
            token.type = GREATEREQUAL;
        } else if (token.text == "||") {
            token.type = LOR;
        } else if (token.text == "&&") {
            token.type = LAND;
        } else if (token.text == "!!") {
            token.type = LNOT;
        } else if (token.text == "true") {
            token.type = TRUE;
        } else if (token.text == "false") {
            token.type = FALSE;
        } else {
            makeError(token, "Unexpected token `" + token.text + "`");
            std::exit(1);
        }
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
                stack.push_back(stoi(token.text));
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

                int a = stack.back();
                stack.pop_back();
                int b = stack.back();

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
                int a = stack.back();
                stack.pop_back();
                int b = stack.back();
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
                int a = stack.back();
                stack.pop_back();
                int b = stack.back();
                stack.pop_back();
                stack.push_back(a + b);
            } break;
            case MINUS: {
                if (stack.size() < 2) {
                    makeError(token, "Not enough elements on the stack! Expected 2, got " + std::to_string(stack.size()) + ".");
                    printTokenLineInfo(token);
                    std::exit(1);
                }
                int a = stack.back();
                stack.pop_back();
                int b = stack.back();
                stack.pop_back();
                stack.push_back(b - a);
            } break;
            case MULT: {
                if (stack.size() < 2) {
                    makeError(token, "Not enough elements on the stack! Expected 2, got " + std::to_string(stack.size()) + ".");
                    printTokenLineInfo(token);
                    std::exit(1);
                }
                int a = stack.back();
                stack.pop_back();
                int b = stack.back();
                stack.pop_back();
                stack.push_back(a * b);
            } break;
            case DIV: {
                if (stack.size() < 2) {
                    makeError(token, "Not enough elements on the stack! Expected 2, got " + std::to_string(stack.size()) + ".");
                    printTokenLineInfo(token);
                    std::exit(1);
                }
                int a = stack.back();
                stack.pop_back();
                int b = stack.back();
                stack.pop_back();
                stack.push_back(b / a);
            } break;
            case DUMP: {
                if (stack.size() < 1) {
                    makeError(token, "Not enough elements on the stack! Expected 1, got " + std::to_string(stack.size()) + ".");
                    printTokenLineInfo(token);
                    std::exit(1);
                }
                int a = stack.back();
                stack.pop_back();
                std::cout << a << '\n';
            } break;
            case BIND: {
                if (stack.size() < 2) {
                    makeError(token, "Not enough elements on the stack! Expected 2, got " + std::to_string(stack.size()) + ".");
                    printTokenLineInfo(token);
                    std::exit(1);
                }
                int value = stack.back();
                stack.pop_back();
                int name = stack.back();
                stack.pop_back();
                if (storage.contains(name)) {
                    makeError(token, "There is a bind with the name `" + std::to_string(name) + "`!");
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
                int value = stack.back();
                stack.pop_back();
                int name = stack.back();
                stack.pop_back();
                if (!storage.contains(name)) {
                    makeError(token, "There is no bind with the name `" + std::to_string(name) + "`!");
                    std::exit(1);
                }
                storage[name] = value;
            } break;
            case LOAD: {
                int name = stack.back();
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
                int name = stack.back();
                for (Procedure proc : procedureStorage) {
                    if (proc.name == name) {
                        makeError(token, "There is the procedure with the name `" + std::to_string(proc.name) + "`!");
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
                    makeError(token, "Unclosed procedure '" + std::to_string(name) + "'");
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
                int name = stack.back();
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
                    makeError(token, "No such procedure '" + std::to_string(name) + "'");
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
                int cond = stack.back();
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
                int a = stack.back();
                stack.pop_back();
                int b = stack.back();
                stack.pop_back();
                stack.push_back(b == a);
            } break;
            case NOTEQUAL: {
                if (stack.size() < 2) {
                    makeError(token, "Not enough elements on the stack!");
                    printTokenLineInfo(token);
                    std::exit(1);
                }
                int a = stack.back();
                stack.pop_back();
                int b = stack.back();
                stack.pop_back();
                stack.push_back(b != a);
            } break;
            case LESS: {
                if (stack.size() < 2) {
                    makeError(token, "Not enough elements on the stack!");
                    printTokenLineInfo(token);
                    std::exit(1);
                }
                int a = stack.back();
                stack.pop_back();
                int b = stack.back();
                stack.pop_back();
                stack.push_back(b < a);
            } break;
            case LESSEQUAL: {
                if (stack.size() < 2) {
                    makeError(token, "Not enough elements on the stack!");
                    printTokenLineInfo(token);
                    std::exit(1);
                }
                int a = stack.back();
                stack.pop_back();
                int b = stack.back();
                stack.pop_back();
                stack.push_back(b <= a);
            } break;
            case GREATER: {
                if (stack.size() < 2) {
                    makeError(token, "Not enough elements on the stack!");
                    printTokenLineInfo(token);
                    std::exit(1);
                }
                int a = stack.back();
                stack.pop_back();
                int b = stack.back();
                stack.pop_back();
                stack.push_back(b > a);
            } break;
            case GREATEREQUAL: {
                if (stack.size() < 2) {
                    makeError(token, "Not enough elements on the stack!");
                    printTokenLineInfo(token);
                    std::exit(1);
                }
                int a = stack.back();
                stack.pop_back();
                int b = stack.back();
                stack.pop_back();
                stack.push_back(b >= a);
            } break;
            case LOR: {
                if (stack.size() < 2) {
                    makeError(token, "Not enough elements on the stack!");
                    printTokenLineInfo(token);
                    std::exit(1);
                }
                int a = stack.back();
                stack.pop_back();
                int b = stack.back();
                stack.pop_back();
                stack.push_back(b || a);
            } break;
            case LAND: {
                if (stack.size() < 2) {
                    makeError(token, "Not enough elements on the stack!");
                    printTokenLineInfo(token);
                    std::exit(1);
                }
                int a = stack.back();
                stack.pop_back();
                int b = stack.back();
                stack.pop_back();
                stack.push_back(b && a);
            } break;
            case LNOT: {
                if (stack.size() < 1) {
                    makeError(token, "Not enough elements on the stack!");
                    printTokenLineInfo(token);
                    std::exit(1);
                }
                int a = stack.back();
                stack.pop_back();
                stack.push_back(!a);
            } break;
            case TRUE: {
                stack.push_back(1);
            } break;
            case FALSE: {
                stack.push_back(0);
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
    for (unsigned int i = 0; i < std::to_string(token.col).length(); i++) {
        std::cout << " ";
    }
    std::cout << " | \033[31m";
    for (unsigned int i = 0; i < token.col; i++) {
        std::cout << " ";
    }
    std::cout << "^\033[0m\n";
}
