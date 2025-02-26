#ifndef NEVERA_UDPLISTENER_H
#define NEVERA_UDPLISTENER_H

#include <Object/Object.h>

class UDPListener : public Object {
    GENERATED_BODY(UDPListener, Object)

public:
    UDPListener() noexcept;
    virtual ~UDPListener() noexcept override;

    size_t read(std::vector<uint8_t>& buffer) const noexcept;

private:
    int socket = -1;
};

#endif //NEVERA_UDPLISTENER_H