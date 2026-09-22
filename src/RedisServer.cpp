#include "../include/RedisServer.h"
#include "../include/CommandHandler.h"
#include "../include/RedisDatabase.h"
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <thread>
#include <vector>
#include <cerrno>
#include <cstring>
#include <signal.h>

static RedisServer *globalServer = nullptr;

void signalHandler(int signum)
{
    if (globalServer)
    {
        std::cout << "Caught signal" << signum << ", shutting down...\n";
        globalServer->shutdown();
    }
    exit(signum);
}

void setupSignalHandler()
{
    signal(SIGINT, signalHandler);
}

RedisServer::RedisServer(int port)
    : port(port), server_socket(-1), running(true)
{
    globalServer = this;
    setupSignalHandler();
}

void RedisServer::shutdown()
{
    running = false;

    // Close server socket
    if (server_socket != -1)
    {
        // server_socket is just an integer representing the socket file descriptor ie. if os give our program discriptor as 3 then close(3)
        close(server_socket);
    }

    std::cout << "Server shutdown completely." << std::endl;
}

void RedisServer::run()
{
    /*
    Create IPv4 TCP socket
    AF_INET-> IPv4
    SOCK_STREAM-> TCP
    0 -> let the OS choose the appropriate protocol for this socket type

    ie. IPv4 + TCP = TCP socket
    */
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket < 0)
    {
        std::cout << "Server Socket Failed";
        return;
    }

    // Allow reuse of the server address so that we did not get Address already in use
    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Configure server IP and port
    sockaddr_in server_addr{}; // contains the server's network address information
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on the machine's available IPv4 interfaces
    server_addr.sin_port = htons(port);       // htons() converts the number into network byte order

    // connects  socket to IP and port
    if (bind(server_socket,
             (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0)
    {
        std::cerr << "Error binding Server Socket: "
                  << std::strerror(errno)
                  << std::endl;

        close(server_socket);
        server_socket = -1;

        return;
    }

    // Start listening for client connections
    if (listen(server_socket, 10) < 0) // here 10 is backlog value. Think of it as the number of connections that can wait in the pending connection queue.
    {
        std::cout << "Error listening on Server Socket ";
        return;
    }

    std::cout << "Server is listening on port : " << port << std::endl;

    /*
    server_socket ---> accepts connections

    client_socket ----> talks to one particular client
    */
    std::vector<std::thread> threads;
    CommandHandler cmdHandler;

    while (running)
    {
        // accept() gives you a new socket specifically for communicating with that client.
        int client_socket = accept(server_socket, nullptr, nullptr);
        if (client_socket < 0)
        {
            if (running)
            {
                std::cout << "Error Accepting Client Connection";
            }

            break;
        }
        threads.emplace_back(
            [client_socket, &cmdHandler]()
            {
            char buffer[1024];
            while(true){
                memset(buffer,0, sizeof(buffer)); // clears the buffer

                // Receive data from this client.
                int bytes=recv(client_socket,buffer, sizeof(buffer)-1,0);
                if(bytes<=0)break;
                std::string request(buffer,bytes);
                std:: string response=cmdHandler.processComand(request);
                send(client_socket,response.c_str(),response.size(),0);
            } 
        close(client_socket); });
    }
    for (auto &t : threads)
    {
        if (t.joinable())
            t.join();
    }

    // Before shutdown, persist the db

    if (RedisDatabase::getInstance().dump("dump.rdb"))
    {
        std::cout << "Database dumped to dump.rdb\n";
    }
    else
    {
        std::cerr << "Error: Dumping database";
    }
}