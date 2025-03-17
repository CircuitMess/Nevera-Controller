#ifndef NEVERA_CONTROLLER_DRIVESCREEN_H
#define NEVERA_CONTROLLER_DRIVESCREEN_H

#include <Services/Battery.h>
#include "UI/Screen.h"
#include "Drive/Bar.h"
#include "Drive/Speed.h"
#include "Services/Feed.h"
#include <Services/ButtonInput.h>
#include "Enums.h"

class DriveScreen : public Screen {
	GENERATED_BODY(DriveScreen, Screen);
	typedef uint16_t Color;

public:
	DriveScreen();

private:
	void onButton(Enum<int> btn, ButtonInput::Action action) noexcept;
	void onDisconnect() noexcept;

	void preRender(Sprite* canvas) override;

	void onBatteryLevelChanged(Battery::Level level) noexcept;
	void onCarBatteryLevelReceived(float level) noexcept;
	void onCarConnectionReceived(float level) noexcept;

	Bar* bar;
	Speed* spd;

	uint64_t startTime; // TODO: only used for elements demoing

	StrongObjectPtr<Feed> feed;

	std::vector<Color> lastFrame;

	/**
	 * 	Direction encoding:
	 * 	0 - forward, -1 - left, 1 - right
	 */
	static float getDirection();

	/**
	 * 	Boost encoding:
	 * 	max: 3, min: -3
	 * 	0 - no movement
	 */
	static float getBoost();

	float dir = 0;
	float boost = 0;

	const std::unordered_map<int, LEDs> LEDMap = {
			{ Button::Forward, LEDs::Boost0 },
			{ Button::Backward, LEDs::Boost1 },
			{ Button::Slider0, LEDs::Slider0 },
			{ Button::Slider1, LEDs::Slider1 },
			{ Button::Slider2, LEDs::Slider2 },
			{ Button::Slider3, LEDs::Slider3 },
			{ Button::Slider4, LEDs::Slider4 },
	};

	static constexpr float MaxBrightness = 0.1f;
};


#endif //NEVERA_CONTROLLER_DRIVESCREEN_H
