#ifndef NEVERA_WIFIACCESSPOINT_H
#define NEVERA_WIFIACCESSPOINT_H

#include <Event/EventBroadcaster.h>
#include <Object/Object.h>

class WiFiAccessPoint : public Object {
    GENERATED_BODY(WiFiAccessPoint, Object)

public:
    enum class ConnectionEventType : uint8_t {
        Connect,
        Disconnect
    };

public:
    DECLARE_EVENT(ConnectionEvent, WiFiAccessPoint, std::string, ConnectionEventType);
    ConnectionEvent OnConnectionEvent{this};

public:
    WiFiAccessPoint();

private:
    void onConnected(const std::string& mac, uint8_t aid, bool isMeshChild);
    void onDisconnected(const std::string& mac, uint8_t aid, bool isMeshChild, uint8_t reason);
};

#endif //NEVERA_WIFIACCESSPOINT_H