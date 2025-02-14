#ifndef NEVERA_CONTROLLER_SPEED_H
#define NEVERA_CONTROLLER_SPEED_H

#include "UI/Element.h"
#include "UI/ImageElement.h"
#include <LovyanGFX.h>

class Speed : public Element {
public:
	Speed();

	void setLevel(uint8_t level);

	void draw(Sprite* canvas) override;

private:
	std::unique_ptr<ImageElement> overlay;

	static constexpr uint16_t ColorBg = lgfx::color565(105, 105, 105);
	static constexpr uint16_t ColorFill = lgfx::color565(45, 165, 113);

	uint8_t targetLevel = 0;
	uint8_t startLevel = 0;
	uint64_t levelTime = 0;
	uint8_t currentLevel();

	static constexpr uint8_t Dots = 27;
	static constexpr uint8_t DotHeight = 4;
	static constexpr uint64_t ChangeDuration = 500;

};


#endif //NEVERA_CONTROLLER_SPEED_H
