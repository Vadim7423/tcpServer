#ifndef SERVER_H
#define SERVER_H

#include <atomic>
#include <thread>
#include <vector>

class Protocol;
class Server
{
public:
    Server(uint16_t port, Protocol& protocol);
    ~Server();
    void start();
    void stop();

private:
    void create_socket();
    void bind_socket();
    void listen_for_connections();
    void accept_connections();
    void handle_client(int client_socket);
    void set_non_blocking(int socket);

    static std::atomic<bool> _stop_server; // Флаг для остановки сервера
    int m_server_socket = -1;                    // Серверный сокет
    std::vector<std::thread> m_client_threads; // Потоки для обработки клиентов

    uint16_t m_port = 0;
    Protocol& m_protocol;
    static constexpr int BUFFER_SIZE = 1024;
};

#endif // SERVER_H
