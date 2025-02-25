#include <iostream>
#include <thread>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

const int PORT = 8080;
const int MAX_CONNECTIONS = 10;

// Function to handle individual client requests
void handle_client(int client_socket) {
    const std::string http_response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 13\r\n"
        "\r\n"
        "Hello, World!";
    
    send(client_socket, http_response.c_str(), http_response.size(), 0);
    close(client_socket);
}

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Failed to create socket\n";
        return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Binding failed\n";
        return 1;
    }

    if (listen(server_socket, MAX_CONNECTIONS) < 0) {
        std::cerr << "Listening failed\n";
        return 1;
    }

    std::cout << "Server is running on port " << PORT << "...\n";

    std::vector<std::thread> threads;
    while (true) {
        int client_socket = accept(server_socket, nullptr, nullptr);
        if (client_socket < 0) {
            std::cerr << "Failed to accept connection\n";
            continue;
        }

        threads.emplace_back(handle_client, client_socket);
    }

    for (auto& th : threads) {
        if (th.joinable()) th.join();
    }

    close(server_socket);
    return 0;
}
