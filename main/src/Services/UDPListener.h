#ifndef NEVERA_UDPLISTENER_H
#define NEVERA_UDPLISTENER_H

#include <Object/Object.h>
#include <Object/Class.h>

class UDPListener : public Object {
    GENERATED_BODY(UDPListener, Object, void)

public:
    UDPListener() noexcept;
    virtual ~UDPListener() noexcept override;

    int64_t read(std::vector<uint8_t>& buffer) const noexcept;
    int64_t read(uint8_t* buffer, size_t count) const noexcept;

private:
    int socket = -1;
};

#endif //NEVERA_UDPLISTENER_H