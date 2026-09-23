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

    // ========== List operations ==========

    std::string handleLLen(
        RedisDatabase &db,
        const std::vector<std::string> &commands);

    std::string handleLPush(
        RedisDatabase &db,
        const std::vector<std::string> &commands);

    std::string handleRPush(
        RedisDatabase &db,
        const std::vector<std::string> &commands);

    std::string handleLPop(
        RedisDatabase &db,
        const std::vector<std::string> &commands);

    std::string handleRPop(
        RedisDatabase &db,
        const std::vector<std::string> &commands);

    std::string handleLRem(
        RedisDatabase &db,
        const std::vector<std::string> &commands);

    std::string handleLIndex(
        RedisDatabase &db,
        const std::vector<std::string> &commands);

    std::string handleLSet(
        RedisDatabase &db,
        const std::vector<std::string> &commands);

    std::string handleLSet(
        RedisDatabase &db,
        const std::vector<std::string> &commands);

    std::string handleHSet(
        RedisDatabase &db,
        const std::vector<std::string> &commands);

    std::string handleHGet(
        RedisDatabase &db,
        const std::vector<std::string> &commands);

    std::string handleHExist(
        RedisDatabase &db,
        const std::vector<std::string> &commands);

    std::string handleHDel(
        RedisDatabase &db,
        const std::vector<std::string> &commands);

    std::string handleHLen(
        RedisDatabase &db,
        const std::vector<std::string> &commands);

    std::string handleHKeys(
        RedisDatabase &db,
        const std::vector<std::string> &commands);

    std::string handleHVals(
        RedisDatabase &db,
        const std::vector<std::string> &commands);

    std::string handleHGetAll(
        RedisDatabase &db,
        const std::vector<std::string> &commands);

    std::string handleHMSet(
        RedisDatabase &db,
        const std::vector<std::string> &commands);
};

#endif