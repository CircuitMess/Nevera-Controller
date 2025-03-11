#include "IntroScreen.h"
#include "PairScreen.h"
#include "UI/AnimElement.h"
#include "Enums.h"
#include <Core/Application.h>
#include <Services/LED/LED.h>
#include <Services/LED/LEDFadeFunction.h>
#include <Services/LED/LEDBlinkFunction.h>

IntroScreen::IntroScreen(){
	auto anim = new AnimElement("/spiffs/Intro.gif");
	anim->setLoopMode(GIF::Single);
	anim->setLoopDoneCb([this](){
		transition(PairScreen::staticClass());
	});

	addElement(anim);
	anim->start();

	auto leds = getApp()->getService<LED<LEDs, RGB_LEDs>>();

	leds->set(LEDs::Slider2, newObject<LEDFadeFunction<LEDs, float>>(nullptr, 0, LEDMaxBrightness, LEDAnimPeriod, LEDAnimRepeatCount));

	leds->set(LEDs::Slider1, newObject<LEDFadeFunction<LEDs, float>>(nullptr, 0, LEDMaxBrightness, LEDAnimPeriod, LEDAnimRepeatCount, RelativeDelay));
	leds->set(LEDs::Slider3, newObject<LEDFadeFunction<LEDs, float>>(nullptr, 0, LEDMaxBrightness, LEDAnimPeriod, LEDAnimRepeatCount, RelativeDelay));

	leds->set(LEDs::Slider0, newObject<LEDFadeFunction<LEDs, float>>(nullptr, 0, LEDMaxBrightness, LEDAnimPeriod, LEDAnimRepeatCount, 2 * RelativeDelay));
	leds->set(LEDs::Slider4, newObject<LEDFadeFunction<LEDs, float>>(nullptr, 0, LEDMaxBrightness, LEDAnimPeriod, LEDAnimRepeatCount, 2 * RelativeDelay));

	leds->set(LEDs::Boost0, newObject<LEDBlinkFunction<LEDs, float>>(nullptr, LEDMaxBrightness, 1.0f, 0.5f, 5));
	leds->set(LEDs::Boost1, newObject<LEDBlinkFunction<LEDs, float>>(nullptr, LEDMaxBrightness, 1.0f, 0.5f, 5));
}
