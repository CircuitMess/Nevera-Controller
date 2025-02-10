#ifndef NEVERA_CONTROLLER_PAIRSCREEN_H
#define NEVERA_CONTROLLER_PAIRSCREEN_H

#include "UI/Screen.h"
#include "UI/AnimElement.h"
#include "UI/ImageElement.h"
#include <glm.hpp>

class PairScreen : public Screen {
	GENERATED_BODY(PairScreen, Screen);

public:
	PairScreen();

private:
	AnimElement* anim = nullptr;

	ImageElement* bg;
	ImageElement* arrow;

	void update() override;
	void gotoWait();
	void gotoOut();

	static constexpr glm::vec<2, int8_t> ArrowPos = { 34, 55 };
	uint64_t startTime = 0;

};


#endif //NEVERA_CONTROLLER_PAIRSCREEN_H
