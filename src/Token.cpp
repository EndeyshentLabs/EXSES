#include <Token.hpp>
#include <vector>

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
    } else if (str == "^8") {
        return LOAD8;
    } else if (str == "^16") {
        return LOAD16;
    } else if (str == "^32") {
        return LOAD32;
    } else if (str == "^64") {
        return LOAD64;
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
    } else if (str == ">rax") {
        return TORAX;
    } else if (str == ">rbx") {
        return TORBX;
    } else if (str == ">rcx") {
        return TORCX;
    } else if (str == ">rdx") {
        return TORDX;
    } else if (str == ">rsi") {
        return TORSI;
    } else if (str == ">rdi") {
        return TORDI;
    } else if (str == ">rbp") {
        return TORBP;
    } else if (str == ">r8") {
        return TOR8;
    } else if (str == ">r9") {
        return TOR9;
    } else if (str == ">r10") {
        return TOR10;
    } else if (str == "\\syscall") {
        return SYSTEM_SYSCALL;
    }

    return UNDEFINED;
}
