#ifndef REDIS_DATABASE
#define REDIS_DATABASE
#include <string>
#include <mutex>
#include <unordered_map>
#include <vector>

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

    std::mutex db_mutex;
    std::unordered_map<std::string, std::string> kv_store;
    std::unordered_map<std::string, std::vector<std::string>> list_store;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> hash_store;
};
#endif