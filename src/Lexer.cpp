#include <Lexer.hpp>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <format>
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

    this->pos.col = 1;
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
}

#ifdef USE_OLD_LEXER
#warning "Using old lexer, this is deprecated"
void Lexer::lexSource()
{
    while (this->curChar != 0) {
        if (std::isspace(this->curChar)) {
            this->advance();
        } else if (std::isdigit(this->curChar)) {
            Token tok = this->makeNumber();
            this->program.push_back(tok);
            this->advance();
        } else if (std::isalpha(this->curChar) || this->curChar == '_') {
            std::string buf;

            Position startPos(this->pos);

            while (this->curChar != 0 && (std::isdigit(this->curChar) || std::isalpha(this->curChar) || this->curChar == '_' || this->curChar == '!' || this->curChar == '+') && !std::isspace(this->curChar)) {
                buf.push_back(this->curChar);
                this->advance();
            }

            if (buf == "s!") {
                this->program.push_back(Token(startPos, STRING_DUMP, buf, Value(ValueType::NONE, "")));
            } else if (buf == "s+") {
                this->program.push_back(Token(startPos, STRING_PLUS, buf, Value(ValueType::NONE, "")));
            } else if (buf == "_") {
                this->program.push_back(Token(startPos, DROP, buf, Value(ValueType::NONE, "")));
            } else {
                this->program.push_back(Token(startPos, IDENT, buf, Value(ValueType::NONE, buf)));
            }
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
            } else if (this->curChar != ' ') {
                goto UNEXP_CHAR;
            }
            this->program.push_back(Token(startPos, type, type == LAND ? "&&" : "&", Value(ValueType::NONE, "")));
        } else if (this->curChar == '$') {
            Position startPos(this->pos);
            TokenType type = SWAP;
            this->advance();
            if (this->curChar == '&') {
                type = OVER;
                this->advance();
            } else if (this->curChar != ' ') {
                goto UNEXP_CHAR;
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
            } else if (this->curChar != ' ') {
                goto UNEXP_CHAR;
            }
            this->program.push_back(Token(startPos, type, type == LNOT ? "!!" : "!", Value(ValueType::NONE, "")));
            // } else if (this->curChar == 's') {
            //     Position startPos(this->pos);
            //     this->advance();
            //     if (this->curChar == '!') {
            //         this->program.push_back(Token(startPos, STRING_DUMP, "s!", Value(ValueType::NONE, "")));
            //         this->advance();
            //     }
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
                goto UNEXP_CHAR;
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
            } else {
                goto UNEXP_CHAR;
            }
            this->program.push_back(Token(startPos, type, text, Value(ValueType::NONE, "")));
        } else if (this->curChar == '>') {
            Position startPos(this->pos);
            TokenType type = GREATER;
            this->advance();
            if (this->curChar == '=') {
                type = GREATEREQUAL;
                this->advance();
            } else {
                goto UNEXP_CHAR;
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
            this->program.push_back(Token(this->pos, ENDPROC, "\"", Value(ValueType::NONE, "")));
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
        UNEXP_CHAR: // Labels and gotos in 2023 KEKW
            std::printf("%s:%s: ERROR: Unknown token, starts with `%c`\n", this->fileName.c_str(), this->pos.toString().c_str(), this->curChar);
            std::exit(1);
        }
    }
}
#else
void Lexer::lexSource()
{
    while (this->curChar != '\0') {
        if (this->curChar == ' ') {
            this->advance();
        } else if (this->curChar == '#') {
            while (this->curChar != '\0' && this->curChar != '\n') {
                this->advance();
            }
            this->pos.line++;
            this->pos.col = 0;
            this->advance();
        } else if (this->curChar == '\n') {
            this->pos.line++;
            this->pos.col = 0;
            this->advance();
        } else if (std::isdigit(this->curChar)) {
            this->program.push_back(this->makeNumber());
        } else if (this->curChar == '[') {
            this->program.push_back(this->makeString());
            this->advance();
        } else {
            this->program.push_back(this->makeIdentifier());
        }
    }
}
#endif

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

Token Lexer::makeIdentifier()
{
    std::string buf;
    Position startPos(this->pos);

    while (this->curChar != '\0' && !std::isspace(this->curChar)) {
        buf.push_back(this->curChar);
        this->advance();
    }

    if (std::find(Keywords.begin(), Keywords.end(), buf) != Keywords.end()) {
        return Token(startPos, tokenTypeFromString(buf), buf, Value(ValueType::NONE, ""));
    }

    return Token(startPos, IDENT, buf, Value(ValueType::NONE, buf));
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

    bool fail = false;

    for (Block& block : blockStack) {
        std::string msg;
        if (block.type == BlockType::IF || block.type == BlockType::PROC) {
            msg = "Unclosed block";
        } else {
            msg = "Unexpected block closing.";
        }
        std::printf("%s:%s: ERROR: %s.\n", this->fileName.c_str(), this->program[block.ip].pos.toString().c_str(), msg.c_str());
        fail = true;
    }

    if (fail)
        std::exit(1);
}
