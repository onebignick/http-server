// interface that accepts a request and sends a response
class Server {
private:
    virtual void handle_request(int client_socket) = 0;
public:
    virtual void start() = 0;
};