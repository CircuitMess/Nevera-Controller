#include "Comm.h"
#include <Memory/ObjectMemory.h>
#include <CommData.h>
#include "TCPServer.h"

void Comm::tick(float deltaTime) noexcept {
    Super::tick(deltaTime);

    // TODO read incoming data here  (if needed... if not, remove this and set Comm to be object and not async entity)
}

void Comm::sendDriveDir(float dir) noexcept {
    StrongObjectPtr<DriveData> driveData = newObject<DriveData>(this);
    driveData->dataType = DriveData::DataType::Direction;
    driveData->value = dir;

    sendPacket(driveData.get());
}

void Comm::sendDriveSpeed(float speed) noexcept {
    StrongObjectPtr<DriveData> driveData = newObject<DriveData>(this);
    driveData->dataType = DriveData::DataType::Speed;
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
