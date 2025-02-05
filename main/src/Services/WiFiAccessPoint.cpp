#include "WiFiAccessPoint.h"
#include <Periphery/WiFi.h>

WiFiAccessPoint::WiFiAccessPoint() {
    const Application* app = getApp();
    if(app == nullptr) {
        return;
    }

    WiFi* wifi = app->getPeriphery<WiFi>();
    if(wifi == nullptr) {
        return;
    }

    wifi->OnAccessPointConnection.bind(this, &WiFiAccessPoint::onConnected);
}

void WiFiAccessPoint::onConnected(const std::string& mac, uint8_t aid, bool isMeshChild) {
    const Application* app = getApp();
    if(app == nullptr) {
        return;
    }

    WiFi* wifi = app->getPeriphery<WiFi>();
    if(wifi == nullptr) {
        return;
    }

    // TODO this needs to be uncommented once the pair service and pair screen are completed to allow wifi hiding after connection
    /*if(!wifi->isHidden()) {
        wifi->setHidden(true);
    }*/

    OnConnectionEvent.broadcast(mac, ConnectionEventType::Connect);
}

void WiFiAccessPoint::onDisconnected(const std::string& mac, uint8_t aid, bool isMeshChild, uint8_t reason) {
    OnConnectionEvent.broadcast(mac, ConnectionEventType::Disconnect);
}
