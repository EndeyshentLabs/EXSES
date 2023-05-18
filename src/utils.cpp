#include <utils.hpp>

// https://stackoverflow.com/a/64886763
std::vector<std::string> split(const std::string str, const std::string regex_str)
{
    std::regex regexz(regex_str);
    return {std::sregex_token_iterator(str.begin(), str.end(), regexz, -1), std::sregex_token_iterator()};
}
