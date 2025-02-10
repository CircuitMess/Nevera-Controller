#include "IntroScreen.h"
#include "PairScreen.h"
#include "UI/AnimElement.h"

IntroScreen::IntroScreen(){
	auto anim = new AnimElement("/spiffs/Intro.gif");
	anim->setLoopMode(GIF::Single);
	anim->setLoopDoneCb([this](){
		transition(PairScreen::staticClass());
	});

	addElement(anim);
	anim->start();
}
