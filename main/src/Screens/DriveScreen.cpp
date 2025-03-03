#include "DriveScreen.h"
#include <esp_random.h>
#include "Memory/ObjectMemory.h"

DriveScreen::DriveScreen(){
	lastFrame.resize(160*120);

	bar = new Bar();
	addElement(bar);

	spd = new Speed();
	spd->setPos(116, 13);
	addElement(spd);

	startTime = millis();

	feed = newObject<Feed>();

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
