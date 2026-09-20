#include <iostream>
#include <string>
#include <cstring>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    // Default Redis port
    int port = 6379;

    // Allow:
    // ./redis_cli 6380
    if (argc > 1)
    {
        port = std::stoi(argv[1]);
    }

    // -----------------------------------------
    // Create TCP socket
    // -----------------------------------------

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (client_socket < 0)
    {
        std::cerr << "Failed to create socket\n";
        return 1;
    }

    // -----------------------------------------
    // Configure server address
    // -----------------------------------------

    sockaddr_in server_addr{};

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    // Connect to localhost
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0)
    {
        std::cerr << "Invalid server address\n";
        close(client_socket);
        return 1;
    }

    // -----------------------------------------
    // Connect to Redis server
    // -----------------------------------------

    if (connect(
            client_socket,
            (struct sockaddr *)&server_addr,
            sizeof(server_addr)) < 0)
    {
        std::cerr << "Could not connect to server on port "
                  << port << "\n";

        close(client_socket);
        return 1;
    }

    std::cout << "Connected to Redis server on port "
              << port << "\n";

    // -----------------------------------------
    // CLI loop
    // -----------------------------------------

    std::string command;

    while (true)
    {
        std::cout << "redis> ";

        std::getline(std::cin, command);

        // Ctrl+D / EOF
        if (std::cin.eof())
        {
            break;
        }

        // Empty command
        if (command.empty())
        {
            continue;
        }

        // Exit CLI
        if (command == "exit" || command == "quit")
        {
            break;
        }

        // -------------------------------------
        // Send command to server
        // -------------------------------------

        send(
            client_socket,
            command.c_str(),
            command.size(),
            0);

        // -------------------------------------
        // Receive response
        // -------------------------------------

        char buffer[1024];

        memset(buffer, 0, sizeof(buffer));

        int bytes = recv(
            client_socket,
            buffer,
            sizeof(buffer) - 1,
            0);

        if (bytes <= 0)
        {
            std::cout << "Server disconnected\n";
            break;
        }

        std::cout << buffer;
    }

    // -----------------------------------------
    // Close connection
    // -----------------------------------------

    close(client_socket);

    std::cout << "Disconnected from server\n";

    return 0;
}