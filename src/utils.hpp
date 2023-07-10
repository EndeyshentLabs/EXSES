#include <regex>
#include <string>
#include <vector>

std::vector<std::string> split(const std::string str, const std::string regex_str);

inline bool isInteger(const std::string& s)
{
    if (s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+')))
        return false;

    char* p;
    strtol(s.c_str(), &p, 10);

    return (*p == 0);
}
