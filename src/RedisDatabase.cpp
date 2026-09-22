#include "../include/RedisDatabase.h"
#include <mutex>
#include <fstream>
#include <sstream>

RedisDatabase &RedisDatabase::getInstance()
{
    static RedisDatabase instance;
    return instance;
}

bool RedisDatabase::dump(const std::string &filename)
{
    std::lock_guard<std::mutex> lock(db_mutex);
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs)
        return false;

    /*
    Memory

    K-> Key Value
    L-> List
    H-> hash
    */
    for (const auto &kv : kv_store)
    {
        ofs << "K" << kv.first << " " << kv.second << "\n";
    }

    for (const auto &kv : list_store)
    {
        ofs << "L" << kv.first << " ";
        for (const auto &item : kv.second)
            ofs << item;
        ofs << "\n";
    }
    for (const auto &kv : hash_store)
    {
        ofs << "K" << kv.first << " ";
        for (const auto &item : kv.second)
        {
            ofs << item.first << ":" << item.second;
        }
        ofs << "\n";
    }
    return true;
}
bool RedisDatabase::load(const std::string &filename)
{
    std::lock_guard<std::mutex> lock(db_mutex);
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs)
        return false;

    // Removing the existing data
    kv_store.clear();
    hash_store.clear();
    list_store.clear();

    std::string line;

    while (getline(ifs, line))
    {
        std::istringstream iss(line);
        char type;
        iss >> type;
        if (type == 'K')
        {
            std::string key, value;
            iss >> key >> value;
            kv_store[key] = value;
        }
        else if (type == 'L')
        {
            std::vector<std::string> list;
            std::string key;
            iss >> key;
            std::string item;
            while (iss >> item)
            {
                list.push_back(item);
                /* code */
            }
            list_store[key] = list;
        }
        else if (type == 'H')
        {
            std::unordered_map<std::string, std::string> mp;
            std::string key, item;
            iss >> key;

            while (iss >> item)
            {
                auto pos = item.find(':');

                std::string key, value;
                if (pos != std::string::npos)
                {
                    key = item.substr(0, pos);
                    value = item.substr(pos + 1);
                    mp[key] = value;
                }
            }
            hash_store[key] = mp;
        }
    }

    return true;
}