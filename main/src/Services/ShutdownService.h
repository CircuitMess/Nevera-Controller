#ifndef NEVERA_FIRMWARE_SHUTDOWNSERVICE_H
#define NEVERA_FIRMWARE_SHUTDOWNSERVICE_H

#include "Object/Object.h"
#include <esp_sleep.h>
#include "Core/Application.h"
#include "Services/ButtonInput.h"

enum class ShutdownReason : uint8_t {
	Inactivity, Battery
};

class ShutdownService : public AsyncEntity {
	GENERATED_BODY(ShutdownService, AsyncEntity)
public:
	ShutdownService();

	//Hardware shutdown, with notification audio beforehand
	static void shutdown(ShutdownReason reason);

	//To prevent multiple simultaneous shutdown calls
	static SemaphoreHandle_t shutdownSem;

private:
	void tick(float deltaTime) noexcept override;

	static constexpr uint32_t Timeout = 2 * 60000; //2 mins

	SemaphoreHandle_t inactivitySem;

	void inputEvent(Enum<int> btn, ButtonInput::Action action);
};


#endif //NEVERA_FIRMWARE_SHUTDOWNSERVICE_H
