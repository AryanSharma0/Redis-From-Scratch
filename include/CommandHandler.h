#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <string>
#include <vector>

class RedisDatabase;

class CommandHandler
{
public:
    CommandHandler() = default;

    std::string processCommand(
        const std::string &commandLine);

private:
    std::string handlePing(
        const std::vector<std::string> &commands);

    std::string handleEcho(
        const std::vector<std::string> &commands);

    std::string handleSet(
        RedisDatabase &db,
        const std::vector<std::string> &commands);

    std::string handleGet(
        RedisDatabase &db,
        const std::vector<std::string> &commands);

    std::string handleDelete(
        RedisDatabase &db,
        const std::vector<std::string> &commands);

    std::string handleExpire(
        RedisDatabase &db,
        const std::vector<std::string> &commands);

    std::string handleRename(
        RedisDatabase &db,
        const std::vector<std::string> &commands);

    std::string handleKeys(
        RedisDatabase &db);

    std::string handleType(
        RedisDatabase &db,
        const std::vector<std::string> &commands);

    std::string handleFlushAll(
        RedisDatabase &db);
};

#endif