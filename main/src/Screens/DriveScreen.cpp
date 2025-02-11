#include "DriveScreen.h"

DriveScreen::DriveScreen(){
	bar = new Bar();
	addElement(bar);

	spd = new Speed();
	spd->setPos(116, 13);
	addElement(spd);
}

void DriveScreen::update(){

}

void DriveScreen::preRender(Sprite* canvas){
	canvas->fillRect(0, 8, 128, 120, TFT_NAVY);
}
