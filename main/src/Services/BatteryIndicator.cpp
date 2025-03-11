#include "BatteryIndicator.h"
#include "Services/LED/LEDBlinkFunction.h"
#include <utility>

DEFINE_LOG(BatteryIndicator)

BatteryIndicator::BatteryIndicator(StrongObjectPtr<Battery> battery, StrongObjectPtr<LED<LEDs, RGB_LEDs>> leds) : leds(std::move(leds)){
	if(!battery){
		CMF_LOG(BatteryIndicator, LogLevel::Error, "Battery param is nullptr!");
		return;
	}

	//Called for initial setup
	batteryChangeOccurred(battery->getLevel());

	battery->OnLLevelChanged.bind(this, &BatteryIndicator::batteryChangeOccurred);
}

void BatteryIndicator::batteryChangeOccurred(Battery::Level level){
	if(level == Battery::Level::VeryLow){
		leds->off(LEDs::BatteryFull);
		leds->set(LEDs::BatteryLow, newObject<LEDBlinkFunction<LEDs, float>>(
				leds.get(), MaxBrightness, ShortBlinkInterval, BlinkOnDuration, 0));
	}else if(level == Battery::Level::Low){
		leds->off(LEDs::BatteryFull);
		leds->on(LEDs::BatteryLow, MaxBrightness);
	}else{
		leds->on(LEDs::BatteryFull, MaxBrightness * GreenFactor);
		leds->off(LEDs::BatteryLow);
	}
}
