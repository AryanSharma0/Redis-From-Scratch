#include "../include/RedisServer.h"
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

static RedisServer *globalServer = nullptr;

RedisServer::RedisServer(int port)
    : port(port), server_socket(-1), running(true)
{
    globalServer = this;
}

void RedisServer::shutdown()
{
    running = false;

    // Close server socket
    if (server_socket != -1)
    {
        close(server_socket);
    }

    std::cout << "Server shutdown completely." << std::endl;
}

void RedisServer::run()
{
    // Create IPv4 TCP socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket < 0)
    {
        std::cout << "Server Socket Failed";
        return;
    }

    // Allow reuse of the server address
    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Configure server IP and port
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // Bind socket to IP and port
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        std::cout << "Error binding Server Socket ";
        return;
    }

    // Start listening for client connections
    if (listen(server_socket, 10) < 0)
    {
        std::cout << "Error listening on Server Socket ";
        return;
    }

    std::cout << "Server is listening on port : " << port << std::endl;
}