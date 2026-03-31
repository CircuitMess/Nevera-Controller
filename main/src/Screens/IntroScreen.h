#ifndef NEVERA_CONTROLLER_INTROSCREEN_H
#define NEVERA_CONTROLLER_INTROSCREEN_H

#include "UI/Screen.h"

class IntroScreen : public Screen {
	GENERATED_BODY(IntroScreen, Screen, void)

public:
	IntroScreen();

private:
	//For a total of 5 seconds animation
	static constexpr float LEDAnimPeriod = 2.5f;
	static constexpr uint8_t LEDAnimRepeatCount = 2;

	static constexpr float LEDMaxBrightness = 0.05f;
	static constexpr float RelativeDelay = 0.3f;
};

#endif //NEVERA_CONTROLLER_INTROSCREEN_H
