#include "PairScreen.h"
#include <Periphery/WiFi.h>
#include "Services/TCPServer.h"
#include "Services/WiFiAccessPoint.h"
#include "DriveScreen.h"

PairScreen::PairScreen(){
	anim = new AnimElement("/spiffs/Pair/In.gif");
	anim->setLoopMode(GIF::Single);
	anim->setLoopDoneCb([this](){
		gotoWait();
	});

	addElement(anim);
	anim->start();

	const Application* app = getApp();
	if(app == nullptr) {
		return;
	}

	const WiFiAccessPoint* ap = app->getService<WiFiAccessPoint>();
	if(ap == nullptr) {
		return;
	}

	ap->generateNewSSID();

	WiFi* wifi = app->getPeriphery<WiFi>();
	if(wifi == nullptr) {
		return;
	}

	wifi->setHidden(false);
}

PairScreen::~PairScreen() noexcept {
	const Application* app = getApp();
	if(app == nullptr) {
		return;
	}

	WiFi* wifi = app->getPeriphery<WiFi>();
	if(wifi == nullptr) {
		return;
	}

	wifi->setHidden(true);
}

void PairScreen::update(){
	if(startTime == 0) return;

	const auto now = millis();
	const float dt = (float) (now - startTime) / 1000.0f;

	arrow->setY(ArrowPos.y - std::round(std::sin(dt * 4.0f) * 2.0f));

	const Application* app = getApp();
	if(app == nullptr) {
		return;
	}

	WiFi* wifi = app->getPeriphery<WiFi>();
	if(wifi == nullptr) {
		return;
	}

	if(wifi->isHidden()) {
		return;
	}

	TCPServer* tcp = app->getService<TCPServer>();
	if(tcp == nullptr) {
		return;
	}

	if(tcp->isConnected()) {
		return;
	}

	if(tcp->accept()) {
		gotoOut();
		return;
	}
}

void PairScreen::gotoWait(){
	removeElement(anim);
	delete anim;
	anim = nullptr;

	bg = new ImageElement("/spiffs/Pair/Bg.raw", 128, 128);
	addElement(bg);

	arrow = new ImageElement("/spiffs/Pair/Arrow.raw", 11, 10);
	arrow->setPos(ArrowPos.x, ArrowPos.y);
	addElement(arrow);

	startTime = millis();
}

void PairScreen::gotoOut(){
	removeElement(arrow);
	removeElement(bg);
	delete arrow;
	delete bg;
	arrow = bg = nullptr;

	anim = new AnimElement("/spiffs/Pair/Out.gif");
	anim->setLoopMode(GIF::Single);
	anim->setLoopDoneCb([this](){
		transition(DriveScreen::staticClass());
	});

	addElement(anim);
	anim->start();

	startTime = 0;
}
