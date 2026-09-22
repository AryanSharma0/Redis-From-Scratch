#ifndef RESP_PARSER_H
#define RESP_PARSER_H

#include <string>
#include <vector>

class RESPParser
{
public:
    static std::vector<std::string> parse(
        const std::string &input);
};

#endif