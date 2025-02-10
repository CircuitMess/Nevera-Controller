#include "Screen.h"
#include <Core/Application.h>

Screen::~Screen(){
	for(auto* el : elements){
		delete el;
	}
}

const Class* Screen::transitionTo() const noexcept{
	return nextScreen;
}

void Screen::transition(const Class* next) noexcept{
	nextScreen = next;
}

void Screen::addElement(Element* el){
	elements.push_back(el);
}

void Screen::removeElement(Element* el){
	elements.erase(std::remove(elements.begin(), elements.end(), el), elements.end());
}

void Screen::tick(float deltaTime) noexcept{
	SyncEntity::tick(deltaTime);

	if(nextScreen) return;

	const auto time = esp_timer_get_time();

	auto display = getApp()->getDevice<Display>();
	Sprite* canvas = &display->getCanvas();

	update();

	for(auto* el : elements){
		el->loop();
	}

	canvas->clear(TFT_BLACK);

	preRender(canvas);
	render();
	postRender(canvas);

	display->commit();

	const auto loopTime = (micros() - time) / 1000;
	if(loopTime > FrameTime){
		delayMillis(1);
	}else{
		delayMillis(FrameTime - loopTime);
	}
}

void Screen::render(){
	auto display = getApp()->getDevice<Display>();
	Sprite* canvas = &display->getCanvas();

	for(auto* el : elements){
		el->draw(canvas);
	}
}

void Screen::update(){

}

void Screen::preRender(Sprite* canvas){

}

void Screen::postRender(Sprite* canvas){

}
