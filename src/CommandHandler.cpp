#include "../include/CommandHandler.h"
#include "../include/RESPParser.h"
#include "../include/RedisDatabase.h"

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>
#include <sstream>

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

    if (command == "LLEN")
    {
        return handleLLen(db, commands);
    }

    if (command == "LPUSH")
    {
        return handleLPush(db, commands);
    }

    if (command == "RPUSH")
    {
        return handleRPush(db, commands);
    }

    if (command == "LPOP")
    {
        return handleLPop(db, commands);
    }

    if (command == "RPOP")
    {
        return handleRPop(db, commands);
    }

    if (command == "LREM")
    {
        return handleLRem(db, commands);
    }

    if (command == "LINDEX")
    {
        return handleLIndex(db, commands);
    }

    if (command == "LSET")
    {
        return handleLSet(db, commands);
    }

    if (command == "HSET")
        return handleHSet(db, commands);

    if (command == "HGET")
        return handleHGet(db, commands);

    if (command == "HEXISTS")
        return handleHExist(db, commands);

    if (command == "HDEL")
        return handleHDel(db, commands);

    if (command == "HLEN")
        return handleHLen(db, commands);

    if (command == "HKEYS")
        return handleHKeys(db, commands);

    if (command == "HVALS")
        return handleHVals(db, commands);

    if (command == "HGETALL")
        return handleHGetAll(db, commands);

    if (command == "HMSET")
        return handleHMSet(db, commands);

    return "-Error unknown command\r\n";
}

// ========== Common Operations ==========

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
        return "-ERR wrong number of arguments for 'echo'\r\n";
    }

    std::string message;

    for (size_t i = 1; i < commands.size(); ++i)
    {
        if (i > 1)
            message += " ";

        message += commands[i];
    }

    if (message.size() >= 2 &&
        message.front() == '"' &&
        message.back() == '"')
    {
        message = message.substr(1, message.size() - 2);
    }

    return "$" +
           std::to_string(message.size()) +
           "\r\n" +
           message +
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

// ========== Key value operations ==========
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

// ========== List operations ==========
std::string CommandHandler::handleLLen(
    RedisDatabase &db,
    const std::vector<std::string> &commands)
{
    if (commands.size() < 2)
        return "-Error LLen requires key\r\n";
    ssize_t len = db.llen(commands[1]);
    return ":" + std::to_string(len) + "\r\n";
}

std::string CommandHandler::handleLPush(
    RedisDatabase &db,
    const std::vector<std::string> &commands)
{
    if (commands.size() < 3)
        return "-Error LPUSH requires key\r\n";
    std::vector<std::string> list;
    for (size_t i = 2; i < commands.size(); i++)
        list.push_back(commands[i]);

    db.lpush(commands[1], list);
    ssize_t len = db.llen(commands[1]);
    return ":" + std::to_string(len) + "\r\n";
}

std::string CommandHandler::handleRPush(
    RedisDatabase &db,
    const std::vector<std::string> &commands)
{
    if (commands.size() < 3)
        return "-Error RPUSH requires key\r\n";

    std::vector<std::string> list;
    for (size_t i = 2; i < commands.size(); i++)
        list.push_back(commands[i]);

    db.rpush(commands[1], list);
    ssize_t len = db.llen(commands[1]);
    return ":" + std::to_string(len) + "\r\n";
}

std::string CommandHandler::handleLPop(
    RedisDatabase &db,
    const std::vector<std::string> &commands)
{
    if (commands.size() < 2)
        return "-Error LPOP requires key\r\n";
    std::string value;
    if (db.lpop(commands[1], value))
        return "$" + std::to_string(value.size()) + "\r\n" + value + "\r\n";
    return "$-1\r\n";
}
std::string CommandHandler::handleRPop(
    RedisDatabase &db,
    const std::vector<std::string> &commands)
{
    if (commands.size() < 2)
        return "-Error RPOP requires key\r\n";
    std::string value;
    if (db.rpop(commands[1], value))
        return "$" + std::to_string(value.size()) + "\r\n" + value + "\r\n";
    return "$-1\r\n";
}

std::string CommandHandler::handleLRem(
    RedisDatabase &db,
    const std::vector<std::string> &commands)
{
    if (commands.size() < 4)
        return "-Error LREM requires key, count and value\r\n";
    try
    {
        int count = std::stoi(commands[2]);
        int removed = db.lrem(commands[1], count, commands[3]);
        return ":" + std::to_string(removed) + "\r\n";
    }
    catch (const std::exception &)
    {
        return "-Error Invalid count\r\n";
    }
}

std::string CommandHandler::handleLIndex(
    RedisDatabase &db,
    const std::vector<std::string> &commands)
{
    if (commands.size() < 3)
        return "-Error LINDEX requires key and index\r\n";
    try
    {
        int index = std::stoi(commands[2]);
        std::string value;
        if (db.lindex(commands[1], index, value))
            return "$" + std::to_string(value.size()) + "\r\n" + value + "\r\n";
        else
            return "$-1\r\n";
    }
    catch (const std::exception &)
    {
        return "-Error Invalid count\r\n";
    }
}

std::string CommandHandler::handleLSet(
    RedisDatabase &db,
    const std::vector<std::string> &commands)
{
    if (commands.size() < 4)
        return "-Error LINDEX requires key, index and value\r\n";
    try
    {
        int index = std::stoi(commands[2]);
        if (db.lset(commands[1], index, commands[3]))
            return "+OK\r\n";
        else
            return "-Error Index out of range\r\n";
    }
    catch (const std::exception &)
    {
        return "-Error Invalid count\r\n";
    }
}

// ========= Hash Operations =========
std::string CommandHandler::handleHSet(
    RedisDatabase &db,
    const std::vector<std::string> &commands)
{
    if (commands.size() < 4)
        return "-Error HSET requires key, field and value\r\n";
    db.hset(commands[1], commands[2], commands[3]);
    return ":1\r\n";
}

std::string CommandHandler::handleHGet(
    RedisDatabase &db,
    const std::vector<std::string> &commands)
{
    if (commands.size() < 3)
        return "-Error HGET requires key and field \r\n";

    std::string value;
    if (db.hget(commands[1], commands[2], value))
        return "$" + std::to_string(value.size()) + "\r\n" + value + "\r\n";
    else
        return "$-1\r\n";
}

std::string CommandHandler::handleHExist(
    RedisDatabase &db,
    const std::vector<std::string> &commands)
{
    if (commands.size() < 3)
        return "-Error HEXIST requires key and field \r\n";

    bool exist = db.hexist(commands[1], commands[2]);
    return ":" + std::to_string(exist ? 1 : 0) + "\r\n";
}

std::string CommandHandler::handleHDel(
    RedisDatabase &db,
    const std::vector<std::string> &commands)
{
    if (commands.size() < 3)
        return "-Error HDEL requires key and field \r\n";

    bool res = db.hdel(commands[1], commands[2]);
    return ":" + std::to_string(res ? 1 : 0) + "\r\n";
}

std::string CommandHandler::handleHLen(
    RedisDatabase &db,
    const std::vector<std::string> &commands)
{
    if (commands.size() < 2)
        return "-Error HLEN requires key  \r\n";

    ssize_t len = db.hlen(commands[1]);
    return ":" + std::to_string(len) + "\r\n";
}

std::string CommandHandler::handleHKeys(
    RedisDatabase &db,
    const std::vector<std::string> &commands)
{
    if (commands.size() < 2)
        return "-Error HKEYS requires key \r\n";
    auto keys = db.hkeys(commands[1]);
    std::ostringstream oss;
    oss << "*" << keys.size() << "\r\n";
    for (auto &key : keys)
    {
        oss << "$" + std::to_string(key.size()) << "\r\n"
            << key << "\r\n";
    }
    return oss.str();
}

std::string CommandHandler::handleHVals(
    RedisDatabase &db,
    const std::vector<std::string> &commands)
{
    if (commands.size() < 2)
        return "-Error HVALS requires key \r\n";
    auto vals = db.hvals(commands[1]);
    std::ostringstream oss;
    oss << "*" << vals.size() << "\r\n";
    for (auto &val : vals)
    {
        oss << "$" + std::to_string(val.size()) << "\r\n"
            << val << "\r\n";
    }
    return oss.str();
}

std::string CommandHandler::handleHGetAll(
    RedisDatabase &db,
    const std::vector<std::string> &commands)
{
    if (commands.size() < 2)
        return "-Error HGETALL requires key \r\n";
    auto hash = db.hgetall(commands[1]);
    std::ostringstream oss;
    // hash.size() * 2 as key and value we have.
    oss << "*" << hash.size() * 2 << "\r\n";
    for (auto &pair : hash)
    {
        oss << "$" << pair.first.size() << "\r\n"
            << pair.first << "\r\n";
        oss << "$" << pair.second.size() << "\r\n"
            << pair.second << "\r\n";
    }
    return oss.str();
}

std::string CommandHandler::handleHMSet(
    RedisDatabase &db,
    const std::vector<std::string> &commands)
{
    if (commands.size() < 4 || (commands.size() % 2 == 1))
        return "-Error HMSET requires key, field and value pairs \r\n";
    std::vector<std::pair<std::string, std::string>> fieldvalues;
    for (size_t i = 2; i < commands.size(); i += 2)
        fieldvalues.emplace_back(commands[i], commands[i + 1]);

    db.hmset(commands[1], fieldvalues);
    return "+OK\r\n";
}