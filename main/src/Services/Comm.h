#ifndef NEVERA_COMM_H
#define NEVERA_COMM_H

#include <Entity/AsyncEntity.h>
#include <Event/EventBroadcaster.h>

class Comm : public AsyncEntity {
    GENERATED_BODY(Comm, AsyncEntity)

public:
    DECLARE_EVENT(BatteryReceivedEvent, Comm, float);
    BatteryReceivedEvent OnBatteryReceived{this};

public:
    Comm() noexcept;

    virtual void tick(float deltaTime) noexcept override;
    virtual TickType_t getEventScanningTime() const noexcept override;

    void sendDriveDir(float dir) noexcept;
    void sendDriveSpeed(float speed) noexcept;

private:
    static void sendPacket(Object* object) noexcept;

    void onTCPConnected() noexcept;
};

#endif //NEVERA_COMM_H