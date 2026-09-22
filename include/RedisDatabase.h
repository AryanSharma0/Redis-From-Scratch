#ifndef REDIS_DATABASE
#define REDIS_DATABASE
#include <string>

class RedisDatabase
{
public:
    static RedisDatabase &getInstance();

    // Persistance: dump/load db file
    bool dump(const std::string &filename);
    bool load(const std::string &filename);

private:
    RedisDatabase() = default;
    ~RedisDatabase() = default;
    RedisDatabase(const RedisDatabase &) = delete;
    RedisDatabase &operator=(const RedisDatabase &) = delete;
};
#endif