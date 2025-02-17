#ifndef PERSE_MISSIONCTRL_ELEMENT_H
#define PERSE_MISSIONCTRL_ELEMENT_H

#include "Devices/Display.h"

class ElementContainer;

class Element {
public:
	virtual ~Element() = default;

	virtual void draw(Sprite* canvas) = 0;

	virtual void loop(){}

	int16_t getX() const;
	void setX(int16_t x);
	int16_t getY() const;
	void setY(int16_t y);
	void setPos(int16_t x, int16_t y);


private:
	int16_t x = 0, y = 0;

};


#endif //PERSE_MISSIONCTRL_ELEMENT_H
