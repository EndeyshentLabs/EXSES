#include <Token.hpp>
#include <vector>

std::string TokenTypeString[] = {
    "PUSH",
    "STRING",
    "STRING_DUMP",
    "STRING_PLUS",
    "DUP",
    "OVER",
    "DROP",
    "SWAP",
    "PLUS",
    "MINUS",
    "MULT",
    "DIV",
    "DUMP",
    "INPUT",
    "BIND",
    "SAVE",
    "LOAD",
    "TERNARY",
    "MAKEPROC",
    "ENDPROC",
    "INVOKEPROC",
    "IF",
    "ENDIF",
    "EQUAL",
    "NOTEQUAL",
    "LESS",
    "LESSEQUAL",
    "GREATER",
    "GREATEREQUAL",
    "LOR",
    "LAND",
    "LNOT",
    "TRUE",
    "FALSE",

    "IDENT",
};

TokenType tokenTypeFromString(std::string str)
{
    if (str == "s!") {
        return STRING_DUMP;
    } else if (str == "s+") {
        return STRING_PLUS;
    } else if (str == "&") {
        return DUP;
    } else if (str == "$&") {
        return SWAP;
    } else if (str == "_") {
        return DROP;
    } else if (str == "$") {
        return SWAP;
    } else if (str == "+") {
        return PLUS;
    } else if (str == "-") {
        return MINUS;
    } else if (str == "*") {
        return MULT;
    } else if (str == "/") {
        return DIV;
    } else if (str == "!") {
        return DUMP;
    } else if (str == "@") {
        return INPUT;
    } else if (str == "<-") {
        return BIND;
    } else if (str == "<!") {
        return SAVE;
    } else if (str == "^") {
        return LOAD;
    } else if (str == "?:") {
        return TERNARY;
    } else if (str == "'") {
        return MAKEPROC;
    } else if (str == "\"") {
        return ENDPROC;
    } else if (str == ":") {
        return INVOKEPROC;
    } else if (str == "(") {
        return IF;
    } else if (str == ")") {
        return ENDIF;
    } else if (str == "=") {
        return EQUAL;
    } else if (str == "<>") {
        return NOTEQUAL;
    } else if (str == "<") {
        return LESS;
    } else if (str == "<=") {
        return LESSEQUAL;
    } else if (str == ">") {
        return GREATER;
    } else if (str == ">=") {
        return GREATEREQUAL;
    } else if (str == "||") {
        return LOR;
    } else if (str == "&&") {
        return LAND;
    } else if (str == "!!") {
        return LNOT;
    } else if (str == "true") {
        return TRUE;
    } else if (str == "false") {
        return FALSE;
    }

    return UNDEFINED;
}
