#include <CommandHandler.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include "../include/RedisDatabase.h"
/*
    RESP Parser

        *2\r\n
        $4\r\n
        PING\r\n
        $4\r\n
        Test\r\n

    2-> array has 2 elements
    $4-> next string has 4 character

    CRLF = Carriage Return(\r) + Line Feed(\n)

*/

std::vector<std::string> parseRespCmds(const std::string &input)
{
    std::vector<std::string> cmds;

    if (input.empty())
    {
        return cmds;
    }

    //  Handle simple space-separated commands
    if (input[0] != '*')
    {
        std::istringstream iss(input);
        std::string cmd;

        while (iss >> cmd)
        {
            cmds.push_back(cmd);
        }

        return cmds;
    }

    size_t pos = 1;

    // Find CRLF after the number of elements
    size_t crlf = input.find("\r\n", pos);

    if (crlf == std::string::npos)
    {
        return cmds;
    }

    //  Read number of elements

    int numElements = 0;

    try
    {
        numElements = std::stoi(
            input.substr(pos, crlf - pos));
    }
    catch (...)
    {
        return cmds;
    }

    if (numElements < 0)
    {
        return cmds;
    }

    // Move past "*2\r\n"

    pos = crlf + 2;

    //  Parse every RESP element

    for (int i = 0; i < numElements; i++)
    {
        if (pos >= input.size())
        {
            return {};
        }

        // Every bulk string should start with '$'
        // $4\r\nPING\r\n

        if (input[pos] != '$')
        {
            return {};
        }

        // Skip '$'
        pos++;
        crlf = input.find("\r\n", pos);

        if (crlf == std::string::npos)
        {
            return {};
        }

        int len = 0;

        try
        {
            len = std::stoi(
                input.substr(pos, crlf - pos));
        }
        catch (...)
        {
            return {};
        }

        if (len < 0)
        {
            return {};
        }

        // Move past "$4\r\n"

        pos = crlf + 2;
        if (pos + len > input.size())
        {
            return {};
        }

        std::string token = input.substr(pos, len);

        cmds.push_back(token);

        pos += len;

        // ----------------------------------------------
        // Expect CRLF after the string
        // ----------------------------------------------

        if (pos + 2 > input.size())
        {
            return {};
        }

        if (input.substr(pos, 2) != "\r\n")
        {
            return {};
        }

        // Skip CRLF
        pos += 2;
    }

    return cmds;
}

CommandHandler::CommandHandler() {}

std::string CommandHandler::processComand(std::string &commandline)
{
    auto cmds = parseRespCmds(commandline);
    if (cmds.empty())
        return "Error: Empty Commands\r\n";

    std::string cmd = cmds[0];
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
    std::ostringstream response;

    RedisDatabase &db = RedisDatabase::getInstance();

    if (cmd == "PING")
    {
        response << "PONG\r\n";
    }
    else if (cmd == "ECHO")
    {
        if (cmds.size() < 2)
        {
            response << "Error : ECHO requires a message\r\n";
        }
        else
        {
            response << cmds[1] << "\r\n";
        }
        response << "...\r\n";
    }
    else if (cmd == "FLUSHALL")
    {
        db.flushAll();
        response << "Ok\r\n";
    }
    // key value operations
    else if (cmd == "SET")
    {
        if (cmds.size() < 3)
        {
            response << "Error : SET requires key value pair\r\n";
        }
        else
        {
            db.set(cmds[1], cmds[2]);
            response << "Ok\r\n";
        }
    }
    else if (cmd == "GET")
    {
        if (cmds.size() < 2)
        {
            response << "Error : GET requires key name\r\n";
        }
        else
        {
            std::string value;
            if (db.get(cmds[1], value))
                response << value.size() << "\r\n"
                         << value << "\r\n";
            else
                response << "-1\r\n";
        }
    }
    else if (cmd == "KEYS")
    {
        std::vector<std::string> allKeys = db.keys();
        // response << "*" << allKeys.size() << "\r\n";
        for (const auto &key : allKeys)
            response << key << "\r\n";
    }
    else if (cmd == "TYPE")
    {
        if (cmds.size() < 2)
            response << "Error : TYPE requires key name\r\n";
        else
            response << db.type(cmds[1]) << "\r\n ";
    }
    else if (cmd == "DEL" || cmd == "UNLINK")
    {
        if (cmds.size() < 2)
            response << "Error : " << cmd << " requires key name\r\n";
        else
        {
            bool res = db.del(cmds[1]);
            response << (res ? 0 : 1) << "\r\n ";
        }
    }
    else if (cmd == "EXPIRE")
    {
        if (cmds.size() < 3)
            response << "Error : Expire requires key and time in seconds\r\n";
        else
        {
            db.expire(cmds[1], cmds[2]);
            response << "OK" << "\r\n ";
        }
    }
    else if (cmd == "RENAME")
    {
        if (cmds.size() < 3)
            response << "Error : RENAME requires old and new keys also\r\n";
        else
        {
            db.rename(cmds[1], cmds[2]);
            response << "OK " << "\r\n ";
        }
    }
    else
    {
        response << "Error:Unknown command\r\n";
    }
    return response.str();
}