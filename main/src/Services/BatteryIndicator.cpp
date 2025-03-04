#include "BatteryIndicator.h"
#include "Services/LED/LEDBlinkFunction.h"
#include <utility>

BatteryIndicator::BatteryIndicator(StrongObjectPtr<Battery> battery, StrongObjectPtr<LED<LEDs, RGB_LEDs>> leds) :
		AsyncEntity(UpdatePeriod, 4 * 1024, 1, 1), battery(std::move(battery)), leds(std::move(leds)){
}

void BatteryIndicator::tick(float deltaTime) noexcept{
	AsyncEntity::tick(deltaTime);

	if(battery == nullptr){
		return;
	}

	const auto perc = battery->getPerc();

	if(perc <= ShortBlinkThreshold){
		gotoState(State::ShortBlink, perc);
	}else{
		gotoState(State::Continuous, perc);
	}
}

void BatteryIndicator::gotoState(BatteryIndicator::State newState, uint8_t perc){
	if(this->state == newState && this->state != State::Continuous) return;

	this->state = newState;

	switch(newState){
		case State::Continuous:{
			if(perc <= RedThreshold){
				leds->off(LEDs::BatteryFull);
				leds->on(LEDs::BatteryLow, MaxBrightness);
			}else{
				leds->on(LEDs::BatteryFull, MaxBrightness * GreenFactor);
				leds->off(LEDs::BatteryLow);
			}
			break;
		}

		case State::ShortBlink:
			leds->off(LEDs::BatteryFull);
			leds->set(LEDs::BatteryLow, newObject<LEDBlinkFunction<LEDs, float>>(
					leds.get(), MaxBrightness, ShortBlinkInterval, BlinkOnDuration, 0));
			break;
	}
}
