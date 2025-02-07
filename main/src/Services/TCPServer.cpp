#include "TCPServer.h"
#include <lwip/sockets.h>

DEFINE_LOG(TCPServer);

TCPServer::TCPServer() noexcept {
    socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if(socket == -1) {
        CMF_LOG(TCPServer, Error, "Failed to create socket, errno=%d: %s", errno, strerror(errno));
        return;
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(6000);

    inet_pton(AF_INET, "11.0.0.1", &address.sin_addr);

    if(bind(socket, reinterpret_cast<sockaddr*>(&address), sizeof(address)) != -0) {
        CMF_LOG(TCPServer, Error, "Failed to bind address to socket, errno=%d: %s", errno, strerror(errno));
        socket = -1;
        return;
    }

    if(listen(socket, 1) != 0) {
        CMF_LOG(TCPServer, Error, "Failed to listen on socket, errno=%d: %s", errno, strerror(errno));
        return;
    }

    fcntl(socket, F_SETFL, O_NONBLOCK);
}

bool TCPServer::isConnected() const noexcept {
    return client != -1;
}

bool TCPServer::accept() noexcept {
    if(socket == -1) {
        CMF_LOG(TCPServer, Error, "Accept, but socket is invalid");
        return false;
    }

    if(client != -1) {
        CMF_LOG(TCPServer, Error, "Accept, but client is already connected");
        return false;
    }

    sockaddr_in address_client{};
    socklen_t address_client_len = sizeof(address_client);

    if((client = ::accept(socket, reinterpret_cast<sockaddr*>(&address_client), &address_client_len) == -1)) {
        CMF_LOG(TCPServer, Warning, "Cannot accept, errno=%d: %s", errno, strerror(errno));
        return false;
    }

    static constexpr int KeepAlive = 1;
    static constexpr int KeepIdle = 4;
    static constexpr int KeepInterval = 2;
    static constexpr int KeepCount = 2;

    setsockopt(client, SOL_SOCKET, SO_KEEPALIVE, &KeepAlive, sizeof(int));
    setsockopt(client, IPPROTO_TCP, TCP_KEEPIDLE, &KeepIdle, sizeof(int));
    setsockopt(client, IPPROTO_TCP, TCP_KEEPINTVL, &KeepInterval, sizeof(int));
    setsockopt(client, IPPROTO_TCP, TCP_KEEPCNT, &KeepCount, sizeof(int));

    char addr_str[32];
    inet_ntoa_r(address_client.sin_addr, addr_str, sizeof(addr_str)-1);

    CMF_LOG(TCPServer, Info, "Client %s connected", addr_str);

    OnConnected.broadcast();

    return true;
}

void TCPServer::disconnect() noexcept {
    if(client == -1) {
        CMF_LOG(TCPServer, Error, "Disconnect, but client is invalid");
        return;
    }

    close(client);
    client = -1;

    OnDisconnected.broadcast();
}

bool TCPServer::read(std::vector<uint8_t>& buffer) noexcept {
    if(client == -1) {
        CMF_LOG(TCPServer, Error, "Read, but client is invalid");
        return false;
    }

    if(buffer.empty()) {
        return true;
    }

    size_t total = 0;
    while(total < buffer.size()) {
        int now = ::read(client, buffer.data() + total, buffer.size() - total);

        if(now == 0) {
            disconnect();
            return false;
        }else if(now < 0) {
            if(errno == EAGAIN || errno == EWOULDBLOCK) {
                vTaskDelay(1);
                continue;
            }else {
                disconnect();
                return false;
            }
        }

        total += now;
    }

    return true;
}

bool TCPServer::write(const std::vector<uint8_t>& buffer) noexcept {
    if(client == -1) {
        CMF_LOG(TCPServer, Error, "Write, but client is invalid");
        return false;
    }

    if(buffer.empty()) {
        return true;
    }

    size_t total = 0;
    while(total < buffer.size()) {
        int now = ::write(client, buffer.data() + total, buffer.size() - total);

        if(now == 0) {
            disconnect();
            return false;
        }else if(now < 0) {
            if(errno == EAGAIN || errno == EWOULDBLOCK) {
                vTaskDelay(1);
                continue;
            }else {
                disconnect();
                return false;
            }
        }

        total += now;
    }

    return true;
}
