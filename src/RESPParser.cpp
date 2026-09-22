#include "../include/RESPParser.h"

#include <sstream>

std::vector<std::string> RESPParser::parse(
    const std::string &input)
{
    std::vector<std::string> commands;

    if (input.empty())
        return commands;

    // Simple space-separated command
    if (input[0] != '*')
    {
        std::istringstream iss(input);
        std::string token;

        while (iss >> token)
            commands.push_back(token);

        return commands;
    }

    size_t pos = 1;

    size_t crlf = input.find("\r\n", pos);

    if (crlf == std::string::npos)
        return {};

    int count;

    try
    {
        count = std::stoi(input.substr(pos, crlf - pos));
    }
    catch (...)
    {
        return {};
    }

    if (count < 0)
        return {};

    pos = crlf + 2;

    for (int i = 0; i < count; ++i)
    {
        if (pos >= input.size() || input[pos] != '$')
            return {};

        ++pos;

        crlf = input.find("\r\n", pos);

        if (crlf == std::string::npos)
            return {};

        int length;

        try
        {
            length = std::stoi(
                input.substr(pos, crlf - pos));
        }
        catch (...)
        {
            return {};
        }

        if (length < 0)
            return {};

        pos = crlf + 2;

        if (pos + length > input.size())
            return {};

        commands.emplace_back(
            input.substr(pos, length));

        pos += length;

        if (pos + 2 > input.size() ||
            input.compare(pos, 2, "\r\n") != 0)
        {
            return {};
        }

        pos += 2;
    }

    return commands;
}