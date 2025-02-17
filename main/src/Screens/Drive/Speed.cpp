#include "Speed.h"

Speed::Speed(){
	overlay = std::make_unique<ImageElement>("/spiffs/Drive/SpeedBar.raw", 10, 112);
}

void Speed::setLevel(uint8_t level){
	if(level == targetLevel) return;
	startLevel = currentLevel();
	targetLevel = std::clamp((int) level, 0, 100);
	levelTime = millis();
}

void Speed::draw(Sprite* canvas){
	canvas->fillRect(getX(), getY(), 10, 112, ColorBg);

	const int fillDots = std::round((float) Dots * (float) currentLevel() / 100.0f);
	canvas->fillRect(getX() + 3, getY() + 3 + (Dots - fillDots) * DotHeight, 4, fillDots * DotHeight, ColorFill);

	overlay->setPos(getX(), getY());
	overlay->draw(canvas);
}

uint8_t Speed::currentLevel(){
	const auto easeOutCubic = [](float x){
		return 1.0f - std::pow(1.0f - x, 3.0f);
	};

	const float t = std::clamp((float) (millis() - levelTime) / (float) ChangeDuration, 0.0f, 1.0f);
	return startLevel + std::round((float) (targetLevel - startLevel) * easeOutCubic(t));
}
