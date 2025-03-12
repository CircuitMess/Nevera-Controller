#include "Comm.h"
#include <Memory/ObjectMemory.h>
#include "TCPServer.h"

Comm::Comm() noexcept : Super(20, 4 * 1024, 8) {
    data = newObject<CommData>(this);
    sendData = newObject<CommData>(this);
}

void Comm::tick(float deltaTime) noexcept {
    Super::tick(deltaTime);

    const Application* app = getApp();
    if(app == nullptr) {
        return;
    }

    TCPServer* tcp = app->getService<TCPServer>();
    if(tcp == nullptr) {
        return;
    }

    if(!tcp->isConnected()) {
        return;
    }

    std::vector<uint8_t> buffer(sizeof(size_t));
    if(!tcp->read(buffer)) {
        return;
    }

    size_t size = 0;
    memcpy(&size, buffer.data(), sizeof(size_t));

    buffer.resize(size);
    if(!tcp->read(buffer)) {
        return;
    }

    if(!objectFromByteArray<CommData>(data.get(), buffer)) {
        return;
    }

    if(data->dataType == CommData::DataType::Battery) {
        OnBatteryReceived.broadcast(data->value);
    }else if(data->dataType == CommData::DataType::NoFeed){
		OnNoFeedReceived.broadcast(data->value);
	}
}

void Comm::sendDriveDir(float dir) noexcept {
    sendData->dataType = CommData::DataType::Direction;
    sendData->value = dir;

    sendPacket(sendData.get());
}

void Comm::sendDriveSpeed(float speed) noexcept {
    sendData->dataType = CommData::DataType::Speed;
    sendData->value = speed;

    sendPacket(sendData.get());
}

void Comm::sendPacket(Object *object) noexcept {
    if(object == nullptr){
        return;
    }

    const Application* app = getApp();
    if(app == nullptr) {
        return;
    }

    TCPServer* tcp = app->getService<TCPServer>();
    if(tcp == nullptr) {
        return;
    }

    std::vector<uint8_t> data;
    byteArrayFromObject(object, data);

    std::vector<uint8_t> sizeData(sizeof(size_t));
    size_t size = data.size();

    memcpy(sizeData.data(), &size, sizeof(size_t));

    tcp->write(sizeData);
    tcp->write(data);
}
