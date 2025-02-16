#include "server.h"
#include <iostream>
#include <csignal>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h> // Для fcntl
#include "src/protocol.h"

std::atomic<bool> Server::_stop_server = false;

Server::Server(uint16_t port, Protocol& protocol)
    : m_server_socket(-1)
    , m_port(port)
    , m_protocol(protocol)
{
    // Устанавливаем обработчик сигнала SIGINT
    std::signal(SIGINT, [](int signal) {
        if (signal == SIGINT) {
            std::cout << "\nПолучен сигнал SIGINT. Останавливаем сервер..." << std::endl;
            _stop_server = true;
        }
    });
}

Server::~Server() {
    stop();
}

void Server::start() {
    try {
        create_socket();
        bind_socket();
        listen_for_connections();
        std::cout << "Сервер запущен и ожидает подключения..." << std::endl;
        accept_connections();
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        stop();
    }
}

void Server::stop() {
    _stop_server = true;
    if (m_server_socket != -1) {
        close(m_server_socket);
        m_server_socket = -1;
    }
    for (auto& thread : m_client_threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    std::cout << "Сервер остановлен." << std::endl;
}

void Server::create_socket() {
    m_server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_server_socket < 0) {
        throw std::runtime_error("Ошибка при создании сокета: " + std::string(strerror(errno)));
    }

    // Устанавливаем SO_REUSEADDR
    int reuse = 1;
    if (setsockopt(m_server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        throw std::runtime_error("Ошибка при установке SO_REUSEADDR: " + std::string(strerror(errno)));
    }

    // Делаем сокет неблокирующим
    set_non_blocking(m_server_socket);
}

void Server::bind_socket() {
    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(m_port);

    if (bind(m_server_socket, reinterpret_cast<sockaddr*>(&server_address), sizeof(server_address)) < 0) {
        throw std::runtime_error("Ошибка при привязке сокета: " + std::string(strerror(errno)));
    }
}

void Server::listen_for_connections() {
    if (listen(m_server_socket, SOMAXCONN) < 0) {
        throw std::runtime_error("Ошибка при прослушивании сокета: " + std::string(strerror(errno)));
    }
}

void Server::accept_connections() {
    while (!_stop_server) {
        sockaddr_in client_address{};
        socklen_t client_address_size = sizeof(client_address);
        int client_socket = accept(m_server_socket, reinterpret_cast<sockaddr*>(&client_address), &client_address_size);

        if (client_socket < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                // Нет новых подключений, продолжаем цикл
                std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Спим 100 мс
                continue;
            } else {
                std::cerr << "Ошибка при принятии подключения: " << strerror(errno) << std::endl;
                break;
            }
        }

        std::cout << "Новый клиент подключен!" << std::endl;
        m_client_threads.emplace_back(&Server::handle_client, this, client_socket);
    }
}

void Server::handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    while (true) {
        ssize_t bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            break;
        }
        m_protocol.readRequest(client_socket, std::string(buffer, bytes_received));
        // const auto& answer = m_protocol.answer(client_socket);
        // send(client_socket, answer.c_str(), answer.size(), 0);
    }
    std::cout <<  "Клиент № " << client_socket << " отключился" << std::endl;
    close(client_socket);
}

void Server::set_non_blocking(int socket) {
    int flags = fcntl(socket, F_GETFL, 0);
    if (flags == -1) {
        throw std::runtime_error("Ошибка при получении флагов сокета: " + std::string(strerror(errno)));
    }
    if (fcntl(socket, F_SETFL, flags | O_NONBLOCK) == -1) {
        throw std::runtime_error("Ошибка при установке неблокирующего режима: " + std::string(strerror(errno)));
    }
}
