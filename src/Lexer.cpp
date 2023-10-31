#include <Lexer.hpp>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <Parser.hpp>
#include <Token.hpp>
#include <Value.hpp>
#include <utils.hpp>

#include <fmt/format.h>

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

    this->cursor = 0;
    this->curChar = this->source[0];

    this->pos = Position(fileName, 0, 0);
    this->run();
}

void Lexer::advance()
{
    this->cursor++;

    if (this->cursor > this->source.size() - 1) {
        this->curChar = '\0';
        return;
    }

    this->pos.col++;
    this->curChar = this->source.at(this->cursor);
}

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
    unsigned int savedCursor = this->cursor;

    this->advance();

    while (this->curChar != ']' && this->curChar != 0) {
        if (this->curChar == '\\') {
            this->advance();
            switch (this->curChar) {
            case ']':
                buf.push_back(']');
                break;
            case 'n':
                buf.push_back('\n');
                break;
            case 'r':
                buf.push_back('\r');
                break;
            case 't':
                buf.push_back('\t');
                break;
            case 'v':
                buf.push_back('\v');
                break;
            default:
                fmt::print("{}: ERROR: Unknown escaping {}.\n", startPos.toString(), this->curChar);
                break;
            }
            this->advance();
            if (this->curChar == ']' || this->curChar == 0)
                break;
        } else {
            buf.push_back(this->curChar);
            this->advance();
        }
    }

    return Token(startPos, STRING, this->source.substr(savedCursor, this->cursor - savedCursor + 1), Value(ValueType::STRING, buf));
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
        fmt::print("{}: type: {}, text: `{}`, pairIp: {}\n", token.pos.toString(), TokenTypeString[token.type], token.text, token.pairIp);
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
    ENDPROC,
    WHILE,
    DOWHILE,
    ENDWHILE,
    MAKECONSTEXPR,
    ENDCONSTEXPR,
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
                fmt::print("{}: ERROR: `ENDIF` without `IF`\n", token.pos.toString());
                std::exit(1);
            }

            Block block = blockStack.back();
            blockStack.pop_back();

            if (block.type != BlockType::IF) {
                fmt::print("{}: ERROR: `ENDIF` can only close `IF` blocks!\n", token.pos.toString());
                std::exit(1);
            }

            token.pairIp = block.ip;

            program[block.ip].pairIp = ip;
        } else if (token.type == MAKEPROC) {
            blockStack.push_back(Block(BlockType::PROC, ip));
        } else if (token.type == ENDPROC) {
            if (blockStack.size() < 1) {
                fmt::print("{}: ERROR: `ENDPROC` without `MAKEPROC`\n", token.pos.toString());
                std::exit(1);
            }

            Block block = blockStack.back();
            blockStack.pop_back();

            if (block.type != BlockType::PROC) {
                fmt::print("{}: ERROR: `ENDPROC` can only close procedure body!\n", token.pos.toString());
                std::exit(1);
            }

            token.pairIp = block.ip;

            program[block.ip].pairIp = ip;
        } else if (token.type == WHILE) {
            blockStack.push_back(Block(BlockType::WHILE, ip));
        } else if (token.type == DOWHILE) {
            blockStack.push_back(Block(BlockType::DOWHILE, ip));
        } else if (token.type == ENDWHILE) {
            if (blockStack.size() < 2) {
                fmt::print("{}: ERROR: `ENDWHILE` without `WHILE` + `DOWHILE`\n", token.pos.toString());
                std::exit(1);
            }

            Block doWhile = blockStack.back();
            blockStack.pop_back();

            if (doWhile.type != BlockType::DOWHILE) {
                fmt::print("{}: ERROR: `ENDWHILE` can only close `DOWHILE` blocks!\n", token.pos.toString());
                std::exit(1);
            }

            Block whil = blockStack.back();
            blockStack.pop_back();

            if (whil.type != BlockType::WHILE) {
                fmt::print("{}: ERROR: `DOWHILE` can only close `WHILE` blocks!\n", token.pos.toString());
                std::exit(1);
            }

            token.pairIp = whil.ip;
            program[doWhile.ip].pairIp = ip;
            program[whil.ip].pairIp = ip;
        } else if (token.type == MAKECONSTEXPR) {
            blockStack.push_back(Block(BlockType::MAKECONSTEXPR, ip));
        } else if (token.type == ENDCONSTEXPR) {
            if (blockStack.size() < 1) {
                fmt::print("{}: ERROR: `MAKECONSTEXPR` without `ENDCONSTEXPR`\n", token.pos.toString());
            }

            Block makeConstExpr = blockStack.back();
            blockStack.pop_back();

            token.pairIp = makeConstExpr.ip;
            program[makeConstExpr.ip].pairIp = ip;
        }
    }

    bool fail = false;

    for (Block& block : blockStack) {
        std::string msg;
        if (block.type == BlockType::IF || block.type == BlockType::PROC || block.type == BlockType::WHILE || block.type == BlockType::DOWHILE || block.type == BlockType::MAKECONSTEXPR) {
            msg = "Unclosed block";
        } else {
            msg = "Unexpected block closing";
        }
        fmt::print("{}: ERROR: {}.\n", this->program[block.ip].pos.toString(), msg);
        fail = true;
    }

    if (fail)
        std::exit(1);
}
