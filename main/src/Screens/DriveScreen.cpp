#include "DriveScreen.h"
#include <esp_random.h>
#include <Memory/ObjectMemory.h>
#include <Core/Application.h>
#include <Services/TCPServer.h>
#include <Services/LED/LED.h>
#include "Enums.h"
#include "PairScreen.h"
#include "Services/Comm.h"

DriveScreen::DriveScreen(){
	lastFrame.resize(128 * 128);

	bar = new Bar();
	addElement(bar);

	spd = new Speed();
	spd->setPos(116, 13);
	addElement(spd);

	Application* app = getApp();
	if(app == nullptr){
		return;
	}

	Feed* feed = app->getService<Feed>();
	if(feed == nullptr){
		return;
	}

	feed->setWorking(true);

	ButtonInput* input = app->getService<ButtonInput>();
	if(input == nullptr) {
		return;
	}

	input->OnButtonEvent.bind(this, &DriveScreen::onButton);

	TCPServer* server = app->getService<TCPServer>();
	if(server == nullptr) {
		return;
	}

	server->OnDisconnected.bind(this, &DriveScreen::onDisconnect);

	Battery* battery = app->getService<Battery>();
	if(battery == nullptr) {
		return;
	}

	battery->OnLevelChanged.bind(this, &DriveScreen::onBatteryLevelChanged);
	bar->setBattCtrl(static_cast<uint8_t>(battery->getLevel()) * 1.17f);

	Comm* comm = app->getService<Comm>();
	if(comm == nullptr) {
		return;
	}

	comm->OnBatteryReceived.bind(this, &DriveScreen::onCarBatteryLevelReceived);
	comm->OnConnectionReceived.bind(this, &DriveScreen::onCarConnectionReceived);
}

void DriveScreen::onTransitionTo(const Class* next) noexcept {
	Super::onTransitionTo(next);

	Application* app = getApp();
	if(app == nullptr){
		return;
	}

	Feed* feed = app->getService<Feed>();
	if(feed == nullptr){
		return;
	}

	feed->setWorking(false);
}

void DriveScreen::onButton(Enum<int> btn, ButtonInput::Action action) noexcept{
	if(btn == Button::Left || btn == Button::Right){
		const float newDir = getDirection();
		if(newDir == dir) return;

		auto comm = getApp()->getService<Comm>();
		dir = newDir;
		comm->sendDriveDir(dir);
	}else if(btn == Button::Slider0 || btn == Button::Slider1 || btn == Button::Slider2 || btn == Button::Slider3 || btn == Button::Slider4
			 || btn == Button::Backward || btn == Button::Forward || btn == Button::Up || btn == Button::Down){
		const auto newBoost = getBoost();
		if(newBoost == boost) return;

		auto comm = getApp()->getService<Comm>();
		boost = newBoost;

		comm->sendDriveSpeed((boost / 3.0f) * 100.0f);

		spd->setLevel(glm::abs((boost / 3.0f) * 100.0f));
	}

	LED<LEDs, RGB_LEDs>* leds = getApp()->getService<LED<LEDs, RGB_LEDs>>();
	if(leds == nullptr){
		return;
	}

	leds->off(LEDs::Slider0);
	leds->off(LEDs::Slider1);
	leds->off(LEDs::Slider2);
	leds->off(LEDs::Slider3);
	leds->off(LEDs::Slider4);

	if(std::abs(boost) >= 1.0f){
		leds->on(LEDs::Slider2, MaxBrightness);
	}

	if(boost >= 1.5f){
		leds->on(LEDs::Slider1, MaxBrightness);
	}

	if(boost >= 2.0f){
		leds->on(LEDs::Slider0, MaxBrightness);
	}

	if(boost <= -1.5){
		leds->on(LEDs::Slider3, MaxBrightness);
	}

	if(boost <= -2.0){
		leds->on(LEDs::Slider4, MaxBrightness);
	}

	if(!LEDMap.contains(btn)){
		return;
	}

	leds->on(LEDMap.at(btn), action == ButtonInput::Action::Press ? MaxBrightness : 0);
}

void DriveScreen::onDisconnect() noexcept {
	if(transitionTo() != nullptr) {
		return;
	}

	transition(PairScreen::staticClass());
}

void DriveScreen::preRender(Sprite* canvas){
	Application* app = getApp();
	if(app == nullptr){
		return;
	}

	Feed* feed = app->getService<Feed>();
	if(feed == nullptr){
		return;
	}

	feed->nextFrame([this](const Color* frame){
		if(frame == nullptr){
			return;
		}

		memcpy(lastFrame.data(), frame, 128 * 128 * 2);
	});

	canvas->pushImage(0, 0, 128, 128, lastFrame.data());
}

void DriveScreen::onBatteryLevelChanged(Battery::Level level) noexcept {
	if(bar == nullptr) {
		return;
	}

	bar->setBattCtrl(static_cast<uint8_t>(level) * 1.17f);
}

void DriveScreen::onCarBatteryLevelReceived(float level) noexcept {
	if(bar == nullptr) {
		return;
	}

	bar->setBattCar(level * 7.99f);
}

void DriveScreen::onCarConnectionReceived(float level) noexcept {
	if(bar == nullptr) {
		return;
	}

	bar->setSignal(level * 3.99f);
}

float DriveScreen::getDirection(){
	auto input = getApp()->getService<ButtonInput>();

	if(!input) return 0.0f;

	float dir = 0.0f;
	if(input->getState(Button::Right)) {
		dir -= 1.0f;
	}

	if(input->getState(Button::Left)) {
		dir += 1.0f;
	}

	return dir;
}

/**
 * Boost buttons override and always set the maximum value (3 or -3)
 *
 * Slider pads have weights, all forward slider activations are summed up and averaged.
 * Same goes for backward slider pads.
 *
 * Finally, boost is the difference between forward and backward pads.
 *
 */
float DriveScreen::getBoost(){
	auto input = getApp()->getService<ButtonInput>();

	if(!input) return 0;

	float val = (float) ((int) (input->getState(Button::Forward)) - (int) (input->getState(Button::Backward))) * 3.0f;

	if(val != 0) return val;

	static constexpr Button ForwardPads[] = { Button::Up, Button::Slider0, Button::Slider1, Button::Slider2 };
	static constexpr Button BackwardPads[] = { Button::Down, Button::Slider4, Button::Slider3, Button::Slider2 };
	static constexpr int8_t weightMap[] = { 2, 2, 1, 0 };
	uint8_t counter = 0;
	int8_t sum = 0;
	float avg = 0;

	for(uint8_t i = 0; i < 3; i++){
		if(input->getState(ForwardPads[i])){
			sum += weightMap[i];
			counter++;
		}
	}
	if(counter){
		avg = (float) sum / (float) counter;
		val += avg;
	}

	counter = 0;
	sum = 0;
	for(uint8_t i = 0; i < 3; i++){
		if(input->getState(BackwardPads[i])){
			sum += weightMap[i];
			counter++;
		}
	}
	if(counter){
		avg = (float) sum / (float) counter;
		val -= avg;
	}

	return val;
}
