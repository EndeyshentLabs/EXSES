#include <Lexer.hpp>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <Parser.hpp>
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
        std::cerr << "ERROR: Couldn't open file '" << this->fileName << "'!\n";
        std::exit(1);
    }

    this->curChar = this->source[0];

    this->run();
}

void Lexer::advance()
{
    this->cursor++;
    if (this->source[cursor] == 0) { // NOTE: Unsafe
        this->curChar = 0;
        return;
    }
    this->pos.col++;
    this->curChar = this->source.at(this->cursor);
    if (this->curChar == '\n') {
        this->cursor++;
        if (this->source[cursor] == 0) { // NOTE: Unsafe
            this->curChar = 0;
            return;
        }
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
            Token tok = this->makeString();
            this->program.push_back(tok);
            this->advance();
        } else if (this->curChar == '+') {
            Position startPos(this->pos);
            this->program.push_back(Token(startPos, PLUS, "+", Value(ValueType::NONE, "")));
            this->advance();
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
        } else if (this->curChar == 's') {
            Position startPos(this->pos);
            this->advance();
            if (this->curChar == '!') {
                this->program.push_back(Token(startPos, STRING_DUMP, "s!", Value(ValueType::NONE, "")));
                this->advance();
            }
        } else if (this->curChar == 's') {
            Position startPos(this->pos);
            TokenType type = UNDEFINED;
            this->advance();
            if (this->curChar == '!') {
                type = STRING_DUMP;
                this->advance();
            } else if (this->curChar == '+') {
                type = STRING_PLUS;
                this->advance();
            }

            if (type == UNDEFINED) {
                std::printf("%s:%s: ERROR: Unknown token, starts with `s`\n", this->fileName.c_str(), this->pos.toString().c_str());
                std::exit(1);
            }

            this->program.push_back(Token(startPos, type, type == STRING_DUMP ? "s!" : "s+", Value(ValueType::NONE, "")));
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
        } else if (this->curChar == '#') {
            Position savedPos(this->pos);

            while (this->curChar != 0 && this->pos.line == savedPos.line) {
                this->advance();
            }
        } else {
            std::printf("%s:%s: ERROR: Unknown token, starts with `%c`\n", this->fileName.c_str(), this->pos.toString().c_str(), this->curChar);
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

Token Lexer::makeString()
{
    std::string buf;
    Position startPos(this->pos);

    this->advance();

    while (this->curChar != ']' && this->curChar != 0) {
        buf.push_back(this->curChar);
        this->advance();
    }

    return Token(startPos, STRING, buf, Value(ValueType::STRING, buf));
}

void Lexer::run()
{
    this->lexSource();
    this->linkBlocks();

#if defined(DEBUG)
    std::cout << "Program:\n";
    for (Token token : program) {
        std::printf("%s:%s: type: %s, text: `%s`, pairIp: %u\n", this->fileName.c_str(), token.pos.toString().c_str(), TokenTypeString[token.type].c_str(), token.text.c_str(), token.pairIp);
    }
#endif

    Parser parser(this->target, this->fileName, this->program);

    // if (this->target == EXSI) {
    //     this->intrepret();
    // } else {
    //     std::cerr << "Only EXSI target is supported right now!\n";
    //     std::exit(1);
    // }
}

enum class BlockType {
    IF,
    ENDIF,
    PROC,
    ENDPROC
};

struct Block {
    Block(BlockType type, unsigned int ip)
        : type(type)
        , ip(ip)
    {
    }
    BlockType type;
    unsigned int ip;
};

std::vector<Block> blockStack;

void Lexer::linkBlocks()
{
    for (unsigned int ip = 0; ip < program.size(); ip++) {
        Token& token = program[ip];

        if (token.type == IF) {
            blockStack.push_back(Block(BlockType::IF, ip));
        } else if (token.type == ENDIF) {
            if (blockStack.size() < 1) {
                std::printf("%s:%s: ERROR: `ENDIF` without `IF`\n", this->fileName.c_str(), token.pos.toString().c_str());
                std::exit(1);
            }

            Block block = blockStack.back();
            blockStack.pop_back();

            if (block.type != BlockType::IF) {
                std::printf("%s:%s: ERROR: `ENDIF` can only close `IF` blocks!\n", this->fileName.c_str(), token.pos.toString().c_str());
                std::exit(1);
            }

            token.pairIp = block.ip;

            program[block.ip].pairIp = ip;
        } else if (token.type == MAKEPROC) {
            blockStack.push_back(Block(BlockType::PROC, ip));
        } else if (token.type == ENDPROC) {
            if (blockStack.size() < 1) {
                std::printf("%s:%s: ERROR: `ENDPROC` without `MAKEPROC`\n", this->fileName.c_str(), token.pos.toString().c_str());
                std::exit(1);
            }

            Block block = blockStack.back();
            blockStack.pop_back();

            if (block.type != BlockType::PROC) {
                std::printf("%s:%s: ERROR: `ENDPROC` can only close procedure body!\n", this->fileName.c_str(), token.pos.toString().c_str());
                std::exit(1);
            }

            token.pairIp = block.ip;

            program[block.ip].pairIp = ip;
        }
    }
}
