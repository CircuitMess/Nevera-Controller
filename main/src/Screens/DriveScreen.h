#ifndef NEVERA_CONTROLLER_DRIVESCREEN_H
#define NEVERA_CONTROLLER_DRIVESCREEN_H

#include "UI/Screen.h"
#include "Drive/Bar.h"
#include "Drive/Speed.h"

class DriveScreen : public Screen {
	GENERATED_BODY(DriveScreen, Screen);

public:
	DriveScreen();

private:
	void update() override;
	void preRender(Sprite* canvas) override;

	Bar* bar;
	Speed* spd;

	uint64_t startTime; // TODO: only used for elements demoing

};


#endif //NEVERA_CONTROLLER_DRIVESCREEN_H
