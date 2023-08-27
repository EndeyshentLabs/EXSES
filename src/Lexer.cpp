#include <Lexer.hpp>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <Procedure.hpp>
#include <Token.hpp>
#include <Value.hpp>
#include <utils.hpp>

#define makeNote(tokenLike, text)                                                                                                                                    \
    do {                                                                                                                                                             \
        std::cout << fileName + ":" + std::to_string((tokenLike).line + 1) + ":" + std::to_string((tokenLike).col + 1) << ": \033[32mNOTE\033[0m: " << text << '\n'; \
    } while (0)

std::vector<std::string> lines;

Lexer::Lexer(std::string fileName, Target target)
    : target(target)
    , fileName(fileName)
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

void Lexer::advance()
{
    this->cursor++;
    this->pos.col++;
    this->curChar = this->source.at(this->cursor);
    if (this->curChar == '\n') {
        this->cursor++;
        this->pos.line++;
        this->pos.col = 0;
        this->curChar = this->source.at(this->cursor);
    }
}

void Lexer::lexSource()
{
    while (this->curChar != 0) {
        if (std::isspace(this->curChar)) {
            this->advance();
        } else if (std::isdigit(this->curChar)) {
            Token tok = this->makeNumber();
            this->program.push_back(tok);
            this->advance();
        } else if (this->curChar == '[') {
            while (this->curChar != ']' && this->curChar != 0) {
                this->advance();
            }
        } else if (this->curChar == '+') {
            Position startPos(this->pos);
            TokenType type = PLUS;
            this->advance();
            if (this->curChar == '+') {
                type = STRING_PLUS;
                this->advance();
            }
            this->program.push_back(Token(startPos, type, type == STRING_PLUS ? "++" : "+", Value(ValueType::NONE, "")));
        } else if (this->curChar == '-') {
            this->program.push_back(Token(this->pos, MINUS, "-", Value(ValueType::NONE, "")));
            this->advance();
        } else if (this->curChar == '*') {
            this->program.push_back(Token(this->pos, MULT, "*", Value(ValueType::NONE, "")));
            this->advance();
        } else if (this->curChar == '/') {
            this->program.push_back(Token(this->pos, DIV, "/", Value(ValueType::NONE, "")));
            this->advance();
        } else if (this->curChar == '&') {
            Position startPos(this->pos);
            TokenType type = DUP;
            this->advance();
            if (this->curChar == '&') {
                type = LAND;
                this->advance();
            }
            this->program.push_back(Token(startPos, type, type == LAND ? "&&" : "&", Value(ValueType::NONE, "")));
        } else if (this->curChar == '$') {
            Position startPos(this->pos);
            TokenType type = SWAP;
            this->advance();
            if (this->curChar == '&') {
                type = OVER;
                this->advance();
            }
            this->program.push_back(Token(startPos, type, type == OVER ? "$&" : "$", Value(ValueType::NONE, "")));
        } else if (this->curChar == '_') {
            this->program.push_back(Token(this->pos, DROP, "_", Value(ValueType::NONE, "")));
            this->advance();
        } else if (this->curChar == '!') {
            Position startPos(this->pos);
            TokenType type = DUMP;
            this->advance();
            if (this->curChar == '!') {
                type = LNOT;
                this->advance();
            }
            this->program.push_back(Token(startPos, type, type == LNOT ? "!!" : "!", Value(ValueType::NONE, "")));
        } else if (this->curChar == '@') {
            this->program.push_back(Token(this->pos, INPUT, "@", Value(ValueType::NONE, "")));
            this->advance();
        } else if (this->curChar == '=') {
            this->program.push_back(Token(this->pos, EQUAL, "=", Value(ValueType::NONE, "")));
            this->advance();
        } else if (this->curChar == '<') {
            Position startPos(this->pos);
            TokenType type = LESS;
            std::string text = "<";
            this->advance();
            if (this->curChar == '-') {
                type = BIND;
                text = "<-";
                this->advance();
            } else if (this->curChar == '!') {
                type = SAVE;
                text = "<!";
                this->advance();
            } else if (this->curChar == '=') {
                type = LESSEQUAL;
                text = "<=";
                this->advance();
            } else if (this->curChar == '>') {
                type = NOTEQUAL;
                text = "<>";
                this->advance();
            }
            this->program.push_back(Token(startPos, type, text, Value(ValueType::NONE, "")));
        } else if (this->curChar == '>') {
            Position startPos(this->pos);
            TokenType type = GREATER;
            this->advance();
            if (this->curChar == '=') {
                type = GREATEREQUAL;
                this->advance();
            }
            this->program.push_back(Token(startPos, type, type == GREATEREQUAL ? ">=" : ">", Value(ValueType::NONE, "")));
        } else if (this->curChar == '^') {
            this->program.push_back(Token(this->pos, LOAD, "^", Value(ValueType::NONE, "")));
            this->advance();
        } else if (this->curChar == '(') {
            this->program.push_back(Token(this->pos, IF, "(", Value(ValueType::NONE, "")));
            this->advance();
        } else if (this->curChar == ')') {
            this->program.push_back(Token(this->pos, ENDIF, ")", Value(ValueType::NONE, "")));
            this->advance();
        } else if (this->curChar == '?') {
            Position startPos(this->pos);
            this->advance();
            if (this->curChar == ':') {
                this->program.push_back(Token(startPos, TERNARY, "?:", Value(ValueType::NONE, "")));
                this->advance();
            } else {
                printf("ternary\n");
                std::exit(55);
            }
        } else if (this->curChar == ')') {
            this->program.push_back(Token(this->pos, ENDIF, ")", Value(ValueType::NONE, "")));
            this->advance();
        } else if (this->curChar == '\'') {
            this->program.push_back(Token(this->pos, MAKEPROC, "'", Value(ValueType::NONE, "")));
            this->advance();
        } else if (this->curChar == '"') {
            this->program.push_back(Token(this->pos, MAKEPROC, "\"", Value(ValueType::NONE, "")));
            this->advance();
        } else if (this->curChar == ':') {
            this->program.push_back(Token(this->pos, INVOKEPROC, ":", Value(ValueType::NONE, "")));
            this->advance();
        } else if (this->curChar == '|') {
            Position startPos(this->pos);
            this->advance();
            if (this->curChar == '|') {
                this->program.push_back(Token(startPos, LOR, "||", Value(ValueType::NONE, "")));
                this->advance();
            } else {
                printf("lor\n");
                std::exit(55);
            }
        } else {
            std::printf("%s:%s: ERROR: Unknown token, starts with `%c`", this->fileName.c_str(), this->pos.toString().c_str(), this->curChar);
            std::exit(1);
        }
    }
}

Token Lexer::makeNumber()
{
    std::string buf;

    Position startPos(this->pos);

    while (this->curChar != 0 && std::isdigit(this->curChar)) {
        buf.push_back(this->curChar);
        this->advance();
    }

    return Token(startPos, PUSH, buf, Value(ValueType::INT, buf));
}

void Lexer::intrepret(bool inside, std::vector<Token> procBody)
{
    if (!inside && procBody.size() != 0) {
        std::cerr << "ERROR: EndeyshentLabs did something wrong! OMEGALUL\n";
        std::exit(1);
    }

    for (Token token : (inside ? procBody : this->program)) {
        if (!token.enabled && !inside) {
            continue;
        }
        processToken(token, inside);
    }
}

void Lexer::run()
{
    this->lexSource();
#if defined(DEBUG)
    std::cout << "Program:\n";
    for (Token token : program) {
        std::printf("%s: type: %s, text: `%s`\n", tokenLocation(token).c_str(), TokenTypeString[token.type].c_str(), token.text.c_str());
    }
#endif
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
        return PUSH;
    } else if (text == "[+]") {
        return STRING_PLUS;
    } else if (std::regex_match(text, std::regex("\\[.*\\]"))) {
        return STRING;
    } else if (text == "+") {
        return PLUS;
    } else if (text == "-") {
        return MINUS;
    } else if (text == "*") {
        return MULT;
    } else if (text == "/") {
        return DIV;
    } else if (text == "&") {
        return DUP;
    } else if (text == "$&") {
        return OVER;
    } else if (text == "_") {
        return DROP;
    } else if (text == "$") {
        return SWAP;
    } else if (text == "!") {
        return DUMP;
    } else if (text == "@") {
        return INPUT;
    } else if (text == "<-") {
        return BIND;
    } else if (text == "<!") {
        return SAVE;
    } else if (text == "^") {
        return LOAD;
    } else if (text == ".?") {
        return TERNARY;
    } else if (text == "'") {
        return MAKEPROC;
    } else if (text == "\"") {
        return ENDPROC;
    } else if (text == ":") {
        return INVOKEPROC;
    } else if (text == "(") {
        return IF;
    } else if (text == ")") {
        return ENDIF;
    } else if (text == "=") {
        return EQUAL;
    } else if (text == "<>") {
        return NOTEQUAL;
    } else if (text == "<") {
        return LESS;
    } else if (text == "<=") {
        return LESSEQUAL;
    } else if (text == ">") {
        return GREATER;
    } else if (text == ">=") {
        return GREATEREQUAL;
    } else if (text == "||") {
        return LOR;
    } else if (text == "&&") {
        return LAND;
    } else if (text == "!!") {
        return LNOT;
    } else if (text == "true") {
        return TRUE;
    } else if (text == "false") {
        return FALSE;
    }

    return UNDEFINED;
}

void Lexer::makeError(Token token, std::string text)
{
    std::cerr << tokenLocation(token) << ": \033[31mERROR\033[0m: " << text << '\n';
    printTokenLineInfo(token);
}

std::string Lexer::tokenLocation(Token token)
{
    return fileName + ":" + std::to_string(token.pos.line + 1) + ":" + std::to_string(token.pos.col + 1);
}

template <typename T> // This trick is needed for compatibility with both Token and Procedure classes
void printTokenLineInfo(T token)
{
    std::printf("%d | %s\n", token.pos.line + 1, lines[token.pos.line].c_str());
    for (unsigned int i = 0; i < std::to_string(token.pos.line).length() + 1; i++) {
        std::cout << " ";
    }
    std::cout << " | \033[31m";
    for (unsigned int i = 0; i < token.pos.col; i++) {
        std::cout << " ";
    }
    std::cout << "^\033[0m\n";
}

void Lexer::processStringLiteral(Token& token)
{
    token.value.text = token.text.substr(1, token.text.length() - 2);
    if (token.value.text.find('\\') != std::string::npos) {
        unsigned int pos = 0;
        for (auto c : token.value.text) {
            if (c == '\\') {
                Token escapePosTok(Position(token.pos.line, token.pos.col + pos + 1), token.type, token.value.text, Value(ValueType::STRING, token.value.text));
                switch (token.value.text[pos + 1]) {
                case 'n': {
                    token.value.text.replace(pos, 2, "\n");
                } break;
                case 'r': {
                    token.value.text.replace(pos, 2, "\r");
                } break;
                case 's': {
                    token.value.text.replace(pos, 2, " ");
                } break;
                case '+': {
                    token.value.text.replace(pos, 2, "+");
                } break;
                default: {
                    makeError(escapePosTok, "Incomplete escape sequence");
                    std::exit(1);
                }
                }
            }
            pos++;
        }
    }
}

void Lexer::processToken(Token& token, bool inside)
{
    switch (token.type) {
    case PUSH: {
        stack.push_back(Value(ValueType::INT, token.value.text));
    } break;
    case STRING: {
        stack.push_back(Value(ValueType::STRING, token.value.text));
    } break;
    case DUP: {
        stack.push_back(stack.back());
    } break;
    case OVER: {
        if (stack.size() < 2) {
            makeError(token, "Not enough elements on the stack!");
            std::exit(1);
        }

        Value a = stack.back();
        stack.pop_back();
        Value b = stack.back();
        stack.pop_back();

        stack.push_back(b);
        stack.push_back(a);
        stack.push_back(b);
    } break;
    case DROP: {
        if (stack.size() < 1) {
            makeError(token, "Not enough elements on the stack! Expected 1, got " + std::to_string(stack.size()) + ".");
            std::exit(1);
        }

        stack.pop_back();
    } break;
    case SWAP: {
        if (stack.size() < 2) {
            makeError(token, "Not enough elements on the stack! Expected 2, got " + std::to_string(stack.size()) + ".");
            std::exit(1);
        }

        Value a = stack.back();
        stack.pop_back();
        Value b = stack.back();
        stack.pop_back();

        stack.push_back(a);
        stack.push_back(b);
    } break;
    case PLUS: {
        if (stack.size() < 2) {
            makeError(token, "Not enough elements on the stack! Expected 2, got " + std::to_string(stack.size()) + ".");
            std::exit(1);
        }

        Value a = stack.back();
        stack.pop_back();
        Value b = stack.back();
        stack.pop_back();

        if (a.type != ValueType::INT || b.type != ValueType::INT) {
            makeError(token, "Both arguments of `+` operation must be an INTs!");
            std::exit(1);
        }

        stack.push_back(Value(ValueType::INT, std::to_string(std::stol(a.text) + std::stol(b.text))));
    } break;
    case STRING_PLUS: {
        if (stack.size() < 2) {
            makeError(token, "Not enough elements on the stack! Expected 2, got " + std::to_string(stack.size()) + ".");
            std::exit(1);
        }

        Value a = stack.back();
        stack.pop_back();
        Value b = stack.back();
        stack.pop_back();

        stack.push_back(Value(ValueType::STRING, a.text + b.text));
    } break;
    case MINUS: {
        if (stack.size() < 2) {
            makeError(token, "Not enough elements on the stack! Expected 2, got " + std::to_string(stack.size()) + ".");
            std::exit(1);
        }

        Value a = stack.back();
        stack.pop_back();
        Value b = stack.back();
        stack.pop_back();

        if (a.type != ValueType::INT || b.type != ValueType::INT) {
            makeError(token, "Both arguments of `-` operation must be an INTs!");
            std::exit(1);
        }

        stack.push_back(Value(ValueType::INT, std::to_string(std::stol(b.text) - std::stol(a.text))));
    } break;
    case MULT: {
        if (stack.size() < 2) {
            makeError(token, "Not enough elements on the stack! Expected 2, got " + std::to_string(stack.size()) + ".");
            std::exit(1);
        }

        Value a = stack.back();
        stack.pop_back();
        Value b = stack.back();
        stack.pop_back();

        if (a.type != ValueType::INT || b.type != ValueType::INT) {
            makeError(token, "Both arguments of `*` operation must be an INTs!");
            std::exit(1);
        }

        stack.push_back(Value(ValueType::INT, std::to_string(std::stol(a.text) * std::stol(b.text))));
    } break;
    case DIV: {
        if (stack.size() < 2) {
            makeError(token, "Not enough elements on the stack! Expected 2, got " + std::to_string(stack.size()) + ".");
            std::exit(1);
        }

        Value a = stack.back();
        stack.pop_back();
        Value b = stack.back();
        stack.pop_back();

        if (a.type != ValueType::INT || b.type != ValueType::INT) {
            makeError(token, "Both arguments of `/` operation must be an INTs!");
            std::exit(1);
        }

        stack.push_back(Value(ValueType::INT, std::to_string(std::stol(b.text) / std::stol(a.text))));
    } break;
    case DUMP: {
        if (stack.size() < 1) {
            makeError(token, "Not enough elements on the stack! Expected 1, got " + std::to_string(stack.size()) + ".");
            std::exit(1);
        }

        std::string a = stack.back().text;
        stack.pop_back();

        std::cout << a << '\n';
    } break;
    case INPUT: {
        std::string input;
        std::cin >> input;
        stack.push_back(Value(ValueType::STRING, input));
    } break;
    case BIND: {
        if (stack.size() < 2) {
            makeError(token, "Not enough elements on the stack! Expected 2, got " + std::to_string(stack.size()) + ".");
            std::exit(1);
        }

        Value value = stack.back();
        stack.pop_back();
        std::string name = stack.back().text;
        stack.pop_back();

        if (storage.contains(name)) {
            makeError(token, "There is a bind with the name `" + name + "`!");
            std::exit(1);
        }

#if 0
        storage[name] = value;
#else
        storage.insert_or_assign(name, value);
#endif
    } break;
    case SAVE: {
        if (stack.size() < 2) {
            makeError(token, "Not enough elements on the stack! Expected 2, got " + std::to_string(stack.size()) + ".");
            std::exit(1);
        }

        Value value = stack.back();
        stack.pop_back();
        std::string name = stack.back().text;
        stack.pop_back();

        if (!storage.contains(name)) {
            makeError(token, "There is no bind with the name `" + name + "`!");
            std::exit(1);
        }

#if 0
        storage[name] = value;
#else
        storage.insert_or_assign(name, value);
#endif
    } break;
    case LOAD: {
        std::string name = stack.back().text;
        stack.pop_back();

        if (!storage.contains(name)) {
            makeError(token, "There is no bind with that name!");
            std::exit(1);
        }

        stack.push_back(Value(storage.at(name).type, storage.at(name).text));
    } break;
    case TERNARY: {
        if (stack.size() < 3) {
            makeError(token, "Not enough elements on the stack! Expected 3, got " + std::to_string(stack.size()) + ".");
            std::exit(1);
        }

        long cond = std::stol(stack.back().text);
        stack.pop_back();
        Value alt = stack.back();
        stack.pop_back();
        Value main = stack.back();
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
            std::exit(1);
        }

        if (stack.size() < 1) {
            makeError(token, "Not enough elements on the stack! Expected name of the procedure.");
            std::exit(1);
        }

        std::string name = stack.back().text;

        for (Procedure proc : procedureStorage) {
            if (proc.name == name) {
                makeError(token, "There is the procedure with the name `" + proc.name + "`!");

                makeNote(proc, "Defined here.");
                std::exit(1);
            }
        }

        stack.pop_back();

        std::vector<Token> body;
        bool hasEnd = false;

        hasEnd = processFolded(token, MAKEPROC, ENDPROC, body);

        if (!hasEnd) {
            makeError(token, "Unclosed procedure '" + name + "'");
            std::exit(1);
        }

        Procedure proc(token.pos.line, token.pos.col, name, body);
        procedureStorage.push_back(proc);
    } break;
    case ENDPROC: {
        if (token.enabled) {
            makeError(token, "Closing the procedure outside of any procedure!");
            std::exit(1);
        }
    } break;
    case INVOKEPROC: {
        if (stack.size() < 1) {
            makeError(token, "Not enough elements on the stack! Expected name of the procedure to invoke.");
            std::exit(1);
        }

        std::string name = stack.back().text;
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
        if (inside)
            break;
        if (stack.size() < 1) {
            makeError(token, "Not enough elements on the stack! Expected condition.");
            std::exit(1);
        }

        long cond = std::stol(stack.back().text);
        stack.pop_back();

        std::vector<Token> body;
        bool hasEnd = false;

        hasEnd = processFolded(token, IF, ENDIF, body);

        if (!hasEnd) {
            makeError(token, "Unclosed IF");
            std::exit(1);
        }

        if (cond) {
            intrepret(true, body);
        }
    } break;
    case ENDIF: {
        if (token.enabled) {
            makeError(token, "Closing IF statement outside of IF statement");
            std::exit(1);
        }
    } break;
    case EQUAL: {
        if (stack.size() < 2) {
            makeError(token, "Not enough elements on the stack!");
            std::exit(1);
        }

        Value a = stack.back();
        stack.pop_back();
        Value b = stack.back();
        stack.pop_back();

        if (a.type == b.type) {
            stack.push_back(Value(ValueType::INT, std::to_string(b == a)));
        } else {
            stack.push_back(Value(ValueType::INT, "0"));
        }
    } break;
    case NOTEQUAL: {
        if (stack.size() < 2) {
            makeError(token, "Not enough elements on the stack!");
            std::exit(1);
        }

        Value a = stack.back();
        stack.pop_back();
        Value b = stack.back();
        stack.pop_back();

        if (a.type == b.type) {
            stack.push_back(Value(ValueType::INT, std::to_string(b.text != a.text)));
        } else {
            stack.push_back(Value(ValueType::INT, "1"));
        }
    } break;
    case LESS: {
        if (stack.size() < 2) {
            makeError(token, "Not enough elements on the stack!");
            std::exit(1);
        }

        Value a = stack.back();
        stack.pop_back();
        Value b = stack.back();
        stack.pop_back();

        if (a.type != ValueType::INT || b.type != ValueType::INT) {
            makeError(token, "Both arguments of `<` operation must be an INTs!");
            std::exit(1);
        }

        stack.push_back(Value(ValueType::INT, std::to_string(std::stol(b.text) < std::stol(a.text))));
    } break;
    case LESSEQUAL: {
        if (stack.size() < 2) {
            makeError(token, "Not enough elements on the stack!");
            std::exit(1);
        }

        Value a = stack.back();
        stack.pop_back();
        Value b = stack.back();
        stack.pop_back();

        if (a.type != ValueType::INT || b.type != ValueType::INT) {
            makeError(token, "Both arguments of `<=` operation must be an INTs!");
            std::exit(1);
        }

        stack.push_back(Value(ValueType::INT, std::to_string(std::stol(b.text) <= std::stol(a.text))));
    } break;
    case GREATER: {
        if (stack.size() < 2) {
            makeError(token, "Not enough elements on the stack!");
            std::exit(1);
        }

        Value a = stack.back();
        stack.pop_back();
        Value b = stack.back();
        stack.pop_back();

        if (a.type != ValueType::INT || b.type != ValueType::INT) {
            makeError(token, "Both arguments of `>` operation must be an INTs!");
            std::exit(1);
        }

        stack.push_back(Value(ValueType::INT, std::to_string(std::stol(b.text) > std::stol(a.text))));
    } break;
    case GREATEREQUAL: {
        if (stack.size() < 2) {
            makeError(token, "Not enough elements on the stack!");
            std::exit(1);
        }

        Value a = stack.back();
        stack.pop_back();
        Value b = stack.back();
        stack.pop_back();

        if (a.type != ValueType::INT || b.type != ValueType::INT) {
            makeError(token, "Both arguments of `>=` operation must be an INTs!");
            std::exit(1);
        }

        stack.push_back(Value(ValueType::INT, std::to_string(std::stol(b.text) >= std::stol(a.text))));
    } break;
    case LOR: {
        if (stack.size() < 2) {
            makeError(token, "Not enough elements on the stack!");
            std::exit(1);
        }

        Value a = stack.back();
        stack.pop_back();
        Value b = stack.back();
        stack.pop_back();

        if (a.type != ValueType::INT || b.type != ValueType::INT) {
            makeError(token, "Both arguments of `||` operation must be an INTs!");
            std::exit(1);
        }

        stack.push_back(Value(ValueType::INT, std::to_string(std::stol(b.text) || std::stol(a.text))));
    } break;
    case LAND: {
        if (stack.size() < 2) {
            makeError(token, "Not enough elements on the stack!");
            std::exit(1);
        }

        Value a = stack.back();
        stack.pop_back();
        Value b = stack.back();
        stack.pop_back();

        if (a.type != ValueType::INT || b.type != ValueType::INT) {
            makeError(token, "Both arguments of `&&` operation must be an INTs!");
            std::exit(1);
        }

        stack.push_back(Value(ValueType::INT, std::to_string(std::stol(b.text) && std::stol(a.text))));
    } break;
    case LNOT: {
        if (stack.size() < 1) {
            makeError(token, "Not enough elements on the stack!");
            std::exit(1);
        }

        Value a = stack.back();
        stack.pop_back();

        if (a.type != ValueType::INT) {
            makeError(token, "Arguments of `!!` operation must be an INT!");
            std::exit(1);
        }

        stack.push_back(Value(ValueType::INT, std::to_string(!std::stol(a.text))));
    } break;
    case TRUE: {
        stack.push_back(Value(ValueType::INT, std::to_string(1)));
    } break;
    case FALSE: {
        stack.push_back(Value(ValueType::INT, std::to_string(0)));
    } break;
    case UNDEFINED: {
        std::cerr << "ERROR: UNREACHABLE\n";
        std::exit(1);
    } break;
    }
}

bool Lexer::processFolded(Token& token, TokenType startType, TokenType endType, std::vector<Token>& body)
{
    for (Token& op : program) {
        if (op.pos.line < token.pos.line || (op.pos.line == token.pos.line && op.pos.col < token.pos.col) || op.type == startType)
            continue;
        if (op.type == endType && op.enabled) {
            op.enabled = false;
            return true;
            break;
        }
        op.enabled = false;
        body.push_back(op);
    }
    return false;
}

void Lexer::createToken(std::string text, unsigned int col)
{
    Token token(Position(228, col), makeType(text), text, Value(isInteger(text) ? ValueType::INT : ValueType::STRING, text));

    if (token.type == UNDEFINED) {
        makeError(token, "Unexpected token `" + token.text + "`");
        std::exit(1);
    }

    if (token.type == STRING) {
        processStringLiteral(token);
    }

    this->program.push_back(token);
}
