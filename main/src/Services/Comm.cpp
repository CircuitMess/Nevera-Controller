#include "Comm.h"
#include <Memory/ObjectMemory.h>
#include <CommData.h>
#include "TCPServer.h"

Comm::Comm() noexcept {
    const Application* app = getApp();
    if(app == nullptr) {
        return;
    }

    TCPServer* tcp = app->getService<TCPServer>();
    if(tcp == nullptr) {
        return;
    }

    tcp->OnConnected.bind(this, &Comm::onTCPConnected);
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

    StrongObjectPtr<CommData> data = objectFromByteArray<CommData>(buffer, this);
    if(!data.isValid()) {
        return;
    }

    if(data->dataType == CommData::DataType::Battery) {
        OnBatteryReceived.broadcast(data->value);
    }else if(data->dataType == CommData::DataType::NoFeed){
		OnNoFeedReceived.broadcast();
	}
}

TickType_t Comm::getEventScanningTime() const noexcept {
    const Application* app = getApp();
    if(app == nullptr) {
        return portMAX_DELAY;
    }

    const TCPServer* tcp = app->getService<TCPServer>();
    if(tcp == nullptr) {
        return portMAX_DELAY;
    }

    if(tcp->isConnected()) {
        return portMAX_DELAY;
    }

    return 0;
}

void Comm::sendDriveDir(float dir) noexcept {
    StrongObjectPtr<CommData> driveData = newObject<CommData>(this);
    driveData->dataType = CommData::DataType::Direction;
    driveData->value = dir;

    sendPacket(driveData.get());
}

void Comm::sendDriveSpeed(float speed) noexcept {
    StrongObjectPtr<CommData> driveData = newObject<CommData>(this);
    driveData->dataType = CommData::DataType::Speed;
    driveData->value = speed;

    sendPacket(driveData.get());
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

void Comm::onTCPConnected() noexcept {

}
