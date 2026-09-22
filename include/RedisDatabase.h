#ifndef REDIS_DATABASE
#define REDIS_DATABASE
#include <string>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <chrono>
class RedisDatabase
{
public:
    static RedisDatabase &getInstance();

    // Common commands
    bool flushAll();

    // Key value operations
    void set(const std::string &key, const std::string &value);
    bool get(const std::string &key, std::string &value);
    std::vector<std::string> keys();
    std::string type(const std::string &key);
    bool del(const std::string &key);

    bool expire(const std::string &key, const std::string &seconds);

    bool rename(const std::string &oldKey, const std::string &newKey);

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
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> expired_map;
};
#endif