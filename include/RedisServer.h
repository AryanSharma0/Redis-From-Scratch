#ifndef REDIS_SERVER_H
#define REDIS_SERVER_H

#include <atomic>
#include <string>

class RedisServer
{
private:
    int port;
    int server_socket;
    std::atomic<bool> running;

public:
    RedisServer(int port);
    void run();
    void shutdown();
};

#endif