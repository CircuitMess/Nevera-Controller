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

	/**
	 * 	Direction encoding:
	 * 	0 - forward, -1 - left, 1 - right
	 */
	static int8_t getDirection();

	/**
	 * 	Boost encoding:
	 * 	max: 3, min: -3
	 * 	0 - no movement
	 */
	static float getBoost();

	int8_t dir = 0;
	float boost = 0;
};


#endif //NEVERA_CONTROLLER_DRIVESCREEN_H
