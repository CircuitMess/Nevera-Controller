#include "DriveScreen.h"
#include <esp_random.h>

DriveScreen::DriveScreen(){
	bar = new Bar();
	addElement(bar);

	spd = new Speed();
	spd->setPos(116, 13);
	addElement(spd);

	startTime = millis();
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
	canvas->fillRect(0, 8, 128, 120, TFT_NAVY);
}
