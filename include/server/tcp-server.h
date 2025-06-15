#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <pthread.h>

#include "server.h"
#include "request/http.h"
#include "thread-pool/ThreadPool.hpp"

#define MAX_CONNECTIONS 50
#define MAX_MESSAGE_SIZE 2000
#define MAX_QUEUE 50

#define DEFAULT_PORT 5000

typedef struct _GetRequest {
    int client_socket;
} GetRequest;

void* handle_GET(void* args) {
    GetRequest* request = (GetRequest*)args;

    char buffer[MAX_MESSAGE_SIZE];
    int received_bytes = read(request->client_socket, buffer, sizeof(buffer));

    if (received_bytes > 0) {
        std::string response_body = "hello world\n";
        std::string http_response = "HTTP/1.1 200 OK\r\n";
        http_response += "Content-Type: text/plain\r\n";
        http_response += "Content-Length: " + std::to_string(response_body.length()) + "\r\n";
        http_response += "Connection: close\r\n";
        http_response += "\r\n";
        http_response += response_body;

        write(request->client_socket, http_response.c_str(), http_response.size());
        close(request->client_socket);
        delete request;
    }
    return NULL;
}

class TCPServer final : public Server {
private:
    int port;
    sockaddr_in server_address, client_address;
    ThreadPool _pool;

    void handle_request(int client_socket) {
        GetRequest* args = new GetRequest;
        args->client_socket = client_socket;

        Task task;
        task.taskFunction = handle_GET;
        task.args = args;

        _pool.push(task);
    }

public:
    TCPServer() :
        port{DEFAULT_PORT},
        server_address{},
        client_address{},
        _pool{}
    {}

    ~TCPServer() = default;

    void start() {
        int server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket == -1) {
            std::cerr << "Error when creating socket" << std::endl;
            return;
        }

        sockaddr_in server_address;
        server_address.sin_addr.s_addr = INADDR_ANY;
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(port);

        if (bind(server_socket, (sockaddr*)&server_address, sizeof(server_address)) == -1) {
            std::cerr << "Error when binding socket" << std::endl;
            return;
        }

        if (listen(server_socket, MAX_CONNECTIONS) == -1) {
            std::cerr << "Error when listening" << std::endl;
        }
        std::cout << "Now running on: " << INADDR_ANY << ":" << port << std::endl;

        while (true) {
            unsigned int client_address_size = sizeof(client_address);
            int client_socket = accept(server_socket, (sockaddr*)&client_address, &client_address_size);
            if (client_socket == -1) {
                std::cerr << "Error when accepting" << std::endl;
                continue;
            }

            handle_request(client_socket);
            // char buffer[MAX_MESSAGE_SIZE];
            // int received_bytes = read(client_socket, buffer, sizeof(buffer));
            // if (received_bytes >= 0) {
            //     std::cout << received_bytes << std::endl;

            //     std::string response_body = "hello world\n";
            //     std::string http_response = "HTTP/1.1 200 OK\r\n";
            //     http_response += "Content-Type: text/plain\r\n";
            //     http_response += "Content-Length: " + std::to_string(response_body.length()) + "\r\n";
            //     http_response += "Connection: close\r\n";
            //     http_response += "\r\n";
            //     http_response += response_body;

            //     write(client_socket, http_response.c_str(), http_response.size());
            //     close(client_socket);
            // }
        }
    }
};