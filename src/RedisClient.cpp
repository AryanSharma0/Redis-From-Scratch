#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

std::string readLine(const std::string &response, size_t &pos)
{
    size_t end = response.find("\r\n", pos);

    if (end == std::string::npos)
        return "";

    std::string line = response.substr(pos, end - pos);
    pos = end + 2;

    return line;
}

std::string parseRESP(const std::string &response, size_t &pos)
{
    if (pos >= response.size())
        return "";

    char type = response[pos++];

    if (type == '+')
    {
        std::string value = readLine(response, pos);
        return value;
    }

    if (type == '-')
    {
        std::string value = readLine(response, pos);
        return "(error) " + value;
    }

    if (type == ':')
    {
        return readLine(response, pos);
    }

    if (type == '$')
    {
        std::string lengthStr = readLine(response, pos);

        int length;

        try
        {
            length = std::stoi(lengthStr);
        }
        catch (...)
        {
            return "(error) invalid response";
        }

        if (length == -1)
            return "(nil)";

        if (pos + length > response.size())
            return "(error) invalid response";

        std::string value = response.substr(pos, length);
        pos += length;

        if (pos + 2 <= response.size())
            pos += 2;

        return value;
    }

    if (type == '*')
    {
        std::string countStr = readLine(response, pos);

        int count;

        try
        {
            count = std::stoi(countStr);
        }
        catch (...)
        {
            return "(error) invalid response";
        }

        if (count == -1)
            return "(nil)";

        std::ostringstream output;

        for (int i = 0; i < count; ++i)
        {
            std::string value = parseRESP(response, pos);

            output << i + 1 << ") " << value;

            if (i + 1 < count)
                output << "\n";
        }

        return output.str();
    }

    return "(error) unknown RESP type";
}

std::string formatCommand(const std::string &command)
{
    std::vector<std::string> parts;
    std::string part;
    bool inQuotes = false;

    for (char c : command)
    {
        if (c == '"')
        {
            inQuotes = !inQuotes;
            continue;
        }

        if (std::isspace(static_cast<unsigned char>(c)) && !inQuotes)
        {
            parts.push_back(part);
            part.clear();
        }
        else
        {
            part += c;
        }
    }

    if (!part.empty() || inQuotes)
        parts.push_back(part);

    if (parts.empty())
        return "";

    std::string request =
        "*" + std::to_string(parts.size()) + "\r\n";

    for (const auto &arg : parts)
    {
        request +=
            "$" +
            std::to_string(arg.size()) +
            "\r\n" +
            arg +
            "\r\n";
    }

    return request;
}

int main(int argc, char *argv[])
{
    int port = 6379;

    if (argc > 1)
        port = std::stoi(argv[1]);

    int client_socket = socket(
        AF_INET,
        SOCK_STREAM,
        0);

    if (client_socket < 0)
    {
        std::cerr << "Failed to create socket\n";
        return 1;
    }

    sockaddr_in server_addr{};

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(
            AF_INET,
            "127.0.0.1",
            &server_addr.sin_addr) <= 0)
    {
        std::cerr << "Invalid server address\n";
        close(client_socket);
        return 1;
    }

    if (connect(
            client_socket,
            reinterpret_cast<sockaddr *>(&server_addr),
            sizeof(server_addr)) < 0)
    {
        std::cerr << "Could not connect to Redis server on port "
                  << port << "\n";

        close(client_socket);
        return 1;
    }

    std::cout << "Connected to Redis server on port "
              << port << "\n";

    std::string command;

    while (true)
    {
        std::cout << "redis> ";
        std::getline(std::cin, command);

        if (std::cin.eof())
            break;

        if (command.empty())
            continue;

        if (command == "exit" || command == "quit")
            break;

        std::string request = formatCommand(command);

        if (request.empty())
            continue;

        size_t totalSent = 0;

        while (totalSent < request.size())
        {
            ssize_t sent = send(
                client_socket,
                request.data() + totalSent,
                request.size() - totalSent,
                0);

            if (sent <= 0)
            {
                std::cerr << "Failed to send command\n";
                close(client_socket);
                return 1;
            }

            totalSent += sent;
        }

        char buffer[4096];
        std::string response;

        while (true)
        {
            ssize_t bytes = recv(
                client_socket,
                buffer,
                sizeof(buffer),
                0);

            if (bytes <= 0)
            {
                std::cout << "Server disconnected\n";
                close(client_socket);
                return 0;
            }

            response.append(buffer, bytes);

            size_t pos = 0;

            if (response.empty())
                continue;

            char type = response[0];

            if (type == '+' ||
                type == '-' ||
                type == ':' ||
                type == '$' ||
                type == '*')
            {
                std::string result = parseRESP(response, pos);

                if (pos > 0)
                {
                    std::cout << result << "\n";
                    break;
                }
            }
        }
    }

    close(client_socket);

    std::cout << "Disconnected from server\n";

    return 0;
}