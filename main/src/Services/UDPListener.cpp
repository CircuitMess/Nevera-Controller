#include "UDPListener.h"
#include "CommData.h"
#include <Log/Log.h>
#include <lwip/sockets.h>

DEFINE_LOG(UDPListener);

UDPListener::UDPListener() noexcept {
    socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if(socket == -1) {
        CMF_LOG(UDPListener, Error, "Can't create socket, errno=%d: %s", errno, strerror(errno));
        return;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(UDPPort);
    inet_pton(AF_INET, ControllerIP, &addr.sin_addr);
    if(bind(socket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0){
        CMF_LOG(UDPListener, Error, "Can't bind address to socket, errno=%d: %s", errno, strerror(errno));
        socket = -1;
        return;
    }

    int flags = fcntl(socket, F_GETFL, 0);
    fcntl(socket, F_SETFL, flags | O_NONBLOCK);
}

UDPListener::~UDPListener() noexcept {
    close(socket);
}

int32_t UDPListener::read(std::vector<uint8_t>& buffer) const noexcept {
   return read(buffer.data(), buffer.size());
}

int32_t UDPListener::read(uint8_t* buf, size_t count) const{
	if(socket == -1){
		CMF_LOG(UDPListener, Error, "Read, but socket not set-up");
		return false;
	}

	if(count == 0 || buf == nullptr) return 0;

	int bytes = ::recv(socket, buf, count, 0);

	if(bytes < 0){
		if(errno == EAGAIN || errno == EWOULDBLOCK) return 0;

		CMF_LOG(UDPListener, Error, "Read error, errno=%d: %s", errno, strerror(errno));
		return -1;
	}

	return bytes;
}
