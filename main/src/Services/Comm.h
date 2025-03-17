#ifndef NEVERA_COMM_H
#define NEVERA_COMM_H

#include <CommData.h>
#include <Entity/AsyncEntity.h>
#include <Event/EventBroadcaster.h>

class Comm : public AsyncEntity {
    GENERATED_BODY(Comm, AsyncEntity)

public:
    DECLARE_EVENT(BatteryReceivedEvent, Comm, float);
    BatteryReceivedEvent OnBatteryReceived{this};

	/**
	 * Triggered when vehicle detects that no camera is connected.
	 */
	DECLARE_EVENT(NoFeedReceivedEvent, Comm, bool);
	NoFeedReceivedEvent OnNoFeedReceived{this};

    DECLARE_EVENT(ConnectionReceivedEvent, Comm, float);
    ConnectionReceivedEvent OnConnectionReceived{this};

public:
    Comm() noexcept;

    virtual void tick(float deltaTime) noexcept override;

    void sendDriveDir(float dir) noexcept;
    void sendDriveSpeed(float speed) noexcept;

private:
    StrongObjectPtr<CommData> data;
    StrongObjectPtr<CommData> sendData;

private:
    static void sendPacket(Object* object) noexcept;
};

#endif //NEVERA_COMM_H