#ifndef NEVERA_TCPSERVER_H
#define NEVERA_TCPSERVER_H

#include <Event/EventBroadcaster.h>
#include <Object/Object.h>

class TCPServer : public Object {
    GENERATED_BODY(TCPServer, Object)

public:
    DECLARE_EVENT(ConnectedEvent, TCPServer);
    ConnectedEvent OnConnected{this};

    DECLARE_EVENT(DisconnectedEvent, TCPServer);
    DisconnectedEvent OnDisconnected{this};

public:
    TCPServer() noexcept;

    bool isConnected() const noexcept;

    bool accept() noexcept;
    void disconnect() noexcept;

    bool read(std::vector<uint8_t>& buffer) noexcept;
    bool write(const std::vector<uint8_t>& buffer) noexcept;

private:
    int socket = -1;
    int client = -1;
};

#endif //NEVERA_TCPSERVER_H