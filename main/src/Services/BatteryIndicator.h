#ifndef NEVERA_CONTROLLER_BATTERYINDICATOR_H
#define NEVERA_CONTROLLER_BATTERYINDICATOR_H

#include <Entity/AsyncEntity.h>
#include "Battery.h"
#include "Services/LED/LED.h"
#include "Enums.h"
#include <cmath>

class BatteryIndicator : public AsyncEntity {
	GENERATED_BODY(BatteryIndicator, AsyncEntity)
public:
	BatteryIndicator(StrongObjectPtr<Battery> battery = nullptr, StrongObjectPtr<LED<LEDs, RGB_LEDs>> leds = nullptr);

protected:
	void tick(float deltaTime) noexcept override;

private:
	StrongObjectPtr<Battery> battery;
	StrongObjectPtr<LED<LEDs, RGB_LEDs>> leds;

	enum class State : uint8_t {
		Continuous, ShortBlink
	} state = State::Continuous;
	void gotoState(State newState, uint8_t perc);

	static constexpr float ShortBlinkInterval = 0.5f; //[s]
	static constexpr float BlinkOnDuration = 0.1f; //[s]

	static constexpr uint8_t RedThreshold = 15; //red only will long-blink below 15% battery
	static constexpr uint8_t ShortBlinkThreshold = 5; //red only will short-blink below 5% battery

	static constexpr float MaxBrightness = 0.02f;
	static constexpr float GreenFactor = 0.25f; //Green LEDs are visually brighter than red at same value;

	static constexpr uint32_t UpdatePeriod = 5000; //[ms] - 5s
};

#endif //NEVERA_CONTROLLER_BATTERYINDICATOR_H
