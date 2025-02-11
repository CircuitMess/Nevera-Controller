#include "Bar.h"

Bar::Bar(){
	car = std::make_unique<ImageElement>("/spiffs/Drive/Car.raw", 12, 4);
	car->setPos(2, 2);

	battCar = std::make_unique<ImageElement>("/spiffs/Drive/Batt0.raw", 12, 5);
	battCar->setPos(16, 1);

	ctrl = std::make_unique<ImageElement>("/spiffs/Drive/Ctrl.raw", 11, 5);
	ctrl->setPos(101, 1);

	battCtrl = std::make_unique<ImageElement>("/spiffs/Drive/Batt0.raw", 12, 5);
	battCtrl->setPos(114, 1);

	signal = std::make_unique<ImageElement>("/spiffs/Drive/Signal0.raw", 9, 5);
	signal->setPos(61, 1);
}

void Bar::setBattCar(uint8_t level){
	if(level > 7) return;
	char path[32];
	sprintf(path, "/spiffs/Drive/Batt%d.raw", level);
	battCar->setPath(path);
}

void Bar::setBattCtrl(uint8_t level){
	if(level > 7) return;
	char path[32];
	sprintf(path, "/spiffs/Drive/Batt%d.raw", level);
	battCtrl->setPath(path);
}

void Bar::setSignal(uint8_t level){
	if(level > 3) return;
	char path[32];
	sprintf(path, "/spiffs/Drive/Signal%d.raw", level);
	signal->setPath(path);
}

void Bar::draw(Sprite* canvas){
	canvas->fillRect(0, 0, 128, 8, TFT_BLACK);

	for(auto el : { &car, &battCar, &ctrl, &battCtrl, &signal }){
		(*el)->draw(canvas);
	}
}
