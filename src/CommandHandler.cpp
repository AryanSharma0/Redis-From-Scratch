#include "../include/CommandHandler.h"
#include "../include/RESPParser.h"
#include "../include/RedisDatabase.h"

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

std::string CommandHandler::processCommand(
    const std::string &commandLine)
{
    const std::vector<std::string> commands =
        RESPParser::parse(commandLine);

    if (commands.empty())
    {
        return "-Error empty command\r\n";
    }

    std::string command = commands[0];

    // Convert command to uppercase
    std::transform(
        command.begin(),
        command.end(),
        command.begin(),
        [](unsigned char c)
        {
            return static_cast<char>(std::toupper(c));
        });

    RedisDatabase &db =
        RedisDatabase::getInstance();

    /*
        Command routing
    */

    if (command == "PING")
    {
        return handlePing(commands);
    }

    if (command == "ECHO")
    {
        return handleEcho(commands);
    }

    if (command == "SET")
    {
        return handleSet(db, commands);
    }

    if (command == "GET")
    {
        return handleGet(db, commands);
    }

    if (command == "DEL" ||
        command == "UNLINK")
    {
        return handleDelete(db, commands);
    }

    if (command == "EXPIRE")
    {
        return handleExpire(db, commands);
    }

    if (command == "RENAME")
    {
        return handleRename(db, commands);
    }

    if (command == "KEYS")
    {
        return handleKeys(db);
    }

    if (command == "TYPE")
    {
        return handleType(db, commands);
    }

    if (command == "FLUSHALL")
    {
        return handleFlushAll(db);
    }

    return "-Error unknown command\r\n";
}

//    PING
std::string CommandHandler::handlePing(
    const std::vector<std::string> &commands)
{
    if (commands.size() > 1)
    {
        return "$" +
               std::to_string(commands[1].size()) +
               "\r\n" +
               commands[1] +
               "\r\n";
    }

    return "+PONG\r\n";
}

// Echo
std::string CommandHandler::handleEcho(
    const std::vector<std::string> &commands)
{
    if (commands.size() < 2)
    {
        return "-Error wrong number of arguments for 'echo'\r\n";
    }

    const std::string &message = commands[1];

    return "$" +
           std::to_string(message.size()) +
           "\r\n" +
           message +
           "\r\n";
}

// SET key value
std::string CommandHandler::handleSet(
    RedisDatabase &db,
    const std::vector<std::string> &commands)
{
    if (commands.size() < 3)
    {
        return "-Error wrong number of arguments for 'set'\r\n";
    }

    const std::string &key = commands[1];
    const std::string &value = commands[2];

    db.set(key, value);

    return "+OK\r\n";
}

// GET key

std::string CommandHandler::handleGet(
    RedisDatabase &db,
    const std::vector<std::string> &commands)
{
    if (commands.size() < 2)
    {
        return "-Error wrong number of arguments for 'get'\r\n";
    }

    const std::string &key = commands[1];

    std::string value;

    if (!db.get(key, value))
    {
        // RESP Null Bulk String
        return "$-1\r\n";
    }

    return "$" +
           std::to_string(value.size()) +
           "\r\n" +
           value +
           "\r\n";
}

// DEL / UNLINK key

std::string CommandHandler::handleDelete(
    RedisDatabase &db,
    const std::vector<std::string> &commands)
{
    if (commands.size() < 2)
    {
        return "-Error wrong number of arguments for 'del'\r\n";
    }

    const std::string &key = commands[1];

    const bool deleted = db.del(key);

    /*
        Redis integer response:

        1 -> key was deleted
        0 -> key didn't exist
    */

    return ":" +
           std::string(deleted ? "1" : "0") +
           "\r\n";
}

// EXPIRE key seconds
std::string CommandHandler::handleExpire(
    RedisDatabase &db,
    const std::vector<std::string> &commands)
{
    if (commands.size() < 3)
    {
        return "-Error wrong number of arguments for 'expire'\r\n";
    }

    const std::string &key = commands[1];
    const std::string &seconds = commands[2];

    bool result = db.expire(key, seconds);

    return ":" +
           std::string(result ? "1" : "0") +
           "\r\n";
}

// RENAME oldKey newKey

std::string CommandHandler::handleRename(
    RedisDatabase &db,
    const std::vector<std::string> &commands)
{
    if (commands.size() < 3)
    {
        return "-Error wrong number of arguments for 'rename'\r\n";
    }

    const std::string &oldKey = commands[1];
    const std::string &newKey = commands[2];

    bool result =
        db.rename(oldKey, newKey);

    if (!result)
    {
        return "-Error no such key\r\n";
    }

    return "+OK\r\n";
}

// KEYS
std::string CommandHandler::handleKeys(
    RedisDatabase &db)
{
    const std::vector<std::string> keys =
        db.keys();

    /*
        For now we return a RESP array.
    */

    std::string response =
        "*" +
        std::to_string(keys.size()) +
        "\r\n";

    for (const auto &key : keys)
    {
        response +=
            "$" +
            std::to_string(key.size()) +
            "\r\n" +
            key +
            "\r\n";
    }

    return response;
}

// TYPE key
std::string CommandHandler::handleType(
    RedisDatabase &db,
    const std::vector<std::string> &commands)
{
    if (commands.size() < 2)
    {
        return "-Error wrong number of arguments for 'type'\r\n";
    }

    const std::string &key = commands[1];

    const std::string type =
        db.type(key);

    return "+" +
           type +
           "\r\n";
}

// FLUSHALL
std::string CommandHandler::handleFlushAll(
    RedisDatabase &db)
{
    db.flushAll();

    return "+OK\r\n";
}