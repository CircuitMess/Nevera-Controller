#ifndef NEVERA_COMM_H
#define NEVERA_COMM_H

#include <Entity/AsyncEntity.h>

class Comm : public AsyncEntity {
    GENERATED_BODY(Comm, AsyncEntity)

public:
    // TODO events

public:
    virtual void tick(float deltaTime) noexcept override;

    void sendDriveDir(float dir) noexcept;
    void sendDriveSpeed(float speed) noexcept;

private:
    static void sendPacket(Object* object) noexcept;
};

#endif //NEVERA_COMM_H