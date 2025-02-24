#include "UDPListener.h"
#include <Log/Log.h>
#include <lwip/sockets.h>

DEFINE_LOG(UDPListener);

UDPListener::UDPListener() noexcept {
	printf("UDPListener constructor\n");


    socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if(socket == -1) {
        CMF_LOG(UDPListener, Error, "Can't create socket, errno=%d: %s", errno, strerror(errno));
        return;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(6001);
    inet_pton(AF_INET, "11.0.0.2", &addr.sin_addr);
    if(bind(socket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0){
        CMF_LOG(UDPListener, Error, "Can't bind address to socket, errno=%d: %s", errno, strerror(errno));
        socket = -1;
        return;
    }

    int flags = fcntl(socket, F_GETFL, 0);
    fcntl(socket, F_SETFL, flags | O_NONBLOCK);
	printf("UDPListener constructor done\n");
}

UDPListener::~UDPListener() noexcept {
    close(socket);
}

size_t UDPListener::read(std::vector<uint8_t>& buffer) const noexcept {
    if(socket == -1){
        CMF_LOG(UDPListener, Error, "Read, but socket not set-up");
        return -1;
    }

    if(buffer.empty()) {
        return 0;
    }

    int bytes = ::recv(socket, buffer.data(), buffer.size(), 0);

    if(bytes < 0){
        if(errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;
        }

		CMF_LOG(UDPListener, Error, "Read error, errno=%d: %s", errno, strerror(errno));

        return -1;
    }

	printf("buffer resize to %d\n", bytes);

	buffer.resize(bytes);

    return bytes;
}
