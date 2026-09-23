#include "../include/RedisDatabase.h"
#include <mutex>
#include <fstream>
#include <sstream>
#include <chrono>
#include <algorithm>

RedisDatabase &RedisDatabase::getInstance()
{
    static RedisDatabase instance;
    return instance;
}

// Commands operations

bool RedisDatabase::flushAll()
{
    std::lock_guard<std::mutex> lock(db_mutex);
    kv_store.clear();
    list_store.clear();
    hash_store.clear();
    return true;
};

std::vector<std::string> RedisDatabase::keys()
{
    std::lock_guard<std::mutex> lock(db_mutex);
    std::vector<std::string> res;
    for (const auto &pair : kv_store)
    {
        res.push_back(pair.first);
    }
    for (const auto &pair : list_store)
    {
        res.push_back(pair.first);
    }
    for (const auto &pair : hash_store)
    {
        res.push_back(pair.first);
    }
    return res;
};

std::string RedisDatabase::type(const std::string &key)
{
    std::lock_guard<std::mutex> lock(db_mutex);
    if (kv_store.find(key) != kv_store.end())
        return "string";
    else if (list_store.find(key) != list_store.end())
        return "list";
    else if (hash_store.find(key) != hash_store.end())
        return "hash";
    else
        return "none";
};

bool RedisDatabase::del(const std::string &key)
{
    std::lock_guard<std::mutex> lock(db_mutex);
    bool earsed = false;
    earsed |= kv_store.erase(key) > 0;
    earsed |= list_store.erase(key) > 0;
    earsed |= hash_store.erase(key) > 0;

    return earsed;
};

bool RedisDatabase::expire(const std::string &key, const std::string &seconds)
{
    std::lock_guard<std::mutex> lock(db_mutex);
    bool exist = (kv_store.find(key) != kv_store.end()) || (list_store.find(key) != list_store.end()) || (hash_store.find(key) != hash_store.end());
    if (!exist)
        return false;

    expired_map[key] = std::chrono::steady_clock::now() + std::chrono::seconds(std::stoi(seconds));
    return true;
};

bool RedisDatabase::rename(const std::string &oldKey, const std::string &newKey)
{
    std::lock_guard<std::mutex> lock(db_mutex);
    auto kitem = kv_store.find(oldKey);
    auto litem = list_store.find(oldKey);
    auto hitem = hash_store.find(oldKey);
    auto eitem = expired_map.find(oldKey);

    bool found = false;
    if (kitem != kv_store.end())
    {
        kv_store[newKey] = kitem->second;
        kv_store.erase(oldKey);
        found = true;
    }
    else if (litem != list_store.end())
    {
        list_store[newKey] = litem->second;
        list_store.erase(oldKey);
        found = true;
    }
    else if (hitem != hash_store.end())
    {
        hash_store[newKey] = hitem->second;
        hash_store.erase(oldKey);
        found = true;
    }
    if (eitem != expired_map.end())
    {
        expired_map[newKey] = eitem->second;
        expired_map.erase(oldKey);
    }
    return found;
};

// Key value operations
void RedisDatabase::set(const std::string &key, const std::string &value)
{
    std::lock_guard<std::mutex> lock(db_mutex);
    kv_store[key] = value;
};

bool RedisDatabase::get(const std::string &key, std::string &value)
{
    std::lock_guard<std::mutex> lock(db_mutex);
    auto it = kv_store.find(key);
    if (it != kv_store.end())
    {
        value = it->second;
        return true;
    }
    return false;
};

// List operations

ssize_t RedisDatabase::llen(const std::string &key)
{
    std::lock_guard<std::mutex> lock(db_mutex);
    auto it = list_store.find(key);
    if (it != list_store.end())
        return it->second.size();
    return 0;
};

void RedisDatabase::lpush(const std::string &key, const std::vector<std::string> &list)
{
    std::lock_guard<std::mutex> lock(db_mutex);
    for (auto value : list)
        list_store[key].insert(list_store[key].begin(), value);
};

void RedisDatabase::rpush(const std::string &key, const std::vector<std::string> &list)
{
    std::lock_guard<std::mutex> lock(db_mutex);
    for (auto value : list)
        list_store[key].push_back(value);
};

bool RedisDatabase::lpop(const std::string &key, std::string &value)
{
    std::lock_guard<std::mutex> lock(db_mutex);
    auto it = list_store.find(key);
    if (it != list_store.end() && !it->second.empty())
    {
        value = it->second.front();
        it->second.erase(it->second.begin());
        return true;
    }
    return false;
};

bool RedisDatabase::rpop(const std::string &key, std::string &value)
{
    std::lock_guard<std::mutex> lock(db_mutex);
    auto it = list_store.find(key);
    if (it != list_store.end() && !it->second.empty())
    {
        value = it->second.back();
        it->second.pop_back();
        return true;
    }
    return false;
};

int RedisDatabase::lrem(const std::string &key, int count, const std::string &value)
{
    std::lock_guard<std::mutex> lock(db_mutex);
    int removed = 0;
    auto it = list_store.find(key);
    if (it == list_store.end())
        return 0;
    auto &lst = it->second;
    if (count == 0)
    {
        auto new_end = std::remove(lst.begin(), lst.end(), value);
        removed = std::distance(new_end, lst.end());
        lst.erase(new_end, lst.end());
    }
    else if (count > 0)
    {
        //  remove element from head to tail (count is +ive)
        for (auto itr = lst.begin(); itr != lst.end() && removed < count;)
        {
            if (*itr == value)
            {
                itr = lst.erase(itr);
                ++removed;
            }
            else
            {
                ++itr;
            }
        }
    }
    else
    {
        //  remove element from last (count is -ive)
        count = -count;
        for (auto itr = lst.end(); itr != lst.begin() && removed < count;)
        {
            --itr;
            if (*itr == value)
            {
                itr = lst.erase(itr);
                ++removed;
            }
        }
    }
    if (lst.empty())
        list_store.erase(it);
    return removed;
};

bool RedisDatabase::lindex(const std::string &key, int index, std::string &value)
{
    std::lock_guard<std::mutex> lock(db_mutex);
    auto it = list_store.find(key);
    const auto &lst = it->second;
    if (it == list_store.end())
        return false;
    if (index < 0)
        index = static_cast<ssize_t>(lst.size()) + index;

    if (index < 0)
        return false;

    if (static_cast<size_t>(index) >= lst.size())
        return false;

    value = lst[index];
    return true;
};

bool RedisDatabase::lset(const std::string &key, int index, const std::string &value)
{
    std::lock_guard<std::mutex> lock(db_mutex);
    auto it = list_store.find(key);
    auto &lst = it->second;
    if (it == list_store.end())
        return false;
    if (index < 0)
        index = static_cast<ssize_t>(lst.size()) + index;

    if (index < 0)
        return false;

    if (static_cast<size_t>(index) >= lst.size())
        return false;

    lst[index] = value;
    return true;
};

// Data Persistance
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
        ofs << "K " << kv.first << " " << kv.second << "\n";
    }

    for (const auto &kv : list_store)
    {
        ofs << "L " << kv.first << " ";
        for (const auto &item : kv.second)
            ofs << item << " ";
        ofs << "\n";
    }
    for (const auto &kv : hash_store)
    {
        ofs << "H " << kv.first << " ";
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
