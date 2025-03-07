#include "DriveScreen.h"
#include "Services/Comm.h"
#include <esp_random.h>
#include "Memory/ObjectMemory.h"
#include <Core/Application.h>
#include <Services/TCPServer.h>
#include "Enums.h"
#include "PairScreen.h"

DriveScreen::DriveScreen(){
	lastFrame.resize(160 * 120);

	bar = new Bar();
	addElement(bar);

	spd = new Speed();
	spd->setPos(116, 13);
	addElement(spd);

	startTime = millis();

	feed = newObject<Feed>();

	Application* app = getApp();
	if(app == nullptr) {
		return;
	}

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
}

void DriveScreen::onButton(Enum<int> btn, ButtonInput::Action action) noexcept{
	if(btn == Button::Left || btn == Button::Right){
		const auto newDir = DriveScreen::getDirection();
		if(newDir == dir) return;

		auto comm = getApp()->getService<Comm>();
		dir = newDir;
		comm->sendDriveDir(dir);
	}else if(btn == Button::Slider0 || btn == Button::Slider1 || btn == Button::Slider2 || btn == Button::Slider3 || btn == Button::Slider4
			 || btn == Button::Backward || btn == Button::Forward){
		const auto newBoost = DriveScreen::getBoost();
		if(newBoost == boost) return;

		auto comm = getApp()->getService<Comm>();
		boost = newBoost;
		comm->sendDriveSpeed(boost);
	}
}

void DriveScreen::onDisconnect() noexcept {
	if(transitionTo() != nullptr) {
		return;
	}

	transition(PairScreen::staticClass());
}

void DriveScreen::update(){
	if(millis() - startTime >= 2000){
		startTime = millis();
		spd->setLevel(esp_random() % 100);
		bar->setBattCar(esp_random() % 8);
		bar->setBattCtrl(esp_random() % 8);
		bar->setSignal(esp_random() % 4);
	}
}

void DriveScreen::preRender(Sprite* canvas){
	FeedFrame feedFrame;
	feed->nextFrame([this, &feedFrame](const FeedFrame& info, const Color* frame){
		feedFrame = info;

		if(frame == nullptr){
			return;
		}

		memcpy(lastFrame.data(), frame, 160 * 120 * 2);
	});

	canvas->pushImage(0, 0, 160, 120, lastFrame.data());
}

int8_t DriveScreen::getDirection(){
	auto input = getApp()->getService<ButtonInput>();

	if(!input) return 0;

	int8_t dir = (int8_t) (input->getState(Button::Right)) - (int8_t) (input->getState(Button::Left));
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

	static constexpr Button ForwardPads[] = { Button::Slider0, Button::Slider1, Button::Slider2 };
	static constexpr Button BackwardPads[] = { Button::Slider4, Button::Slider3, Button::Slider2 };
	static constexpr int8_t weightMap[] = { 2, 1, 0 };
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
