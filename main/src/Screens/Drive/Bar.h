#ifndef NEVERA_CONTROLLER_BAR_H
#define NEVERA_CONTROLLER_BAR_H

#include "UI/Element.h"
#include "UI/ImageElement.h"

class Bar : public Element {
public:
	Bar();

	void setBattCar(uint8_t level);
	void setBattCtrl(uint8_t level);
	void setSignal(uint8_t level);

private:
	std::unique_ptr<ImageElement> car;
	std::unique_ptr<ImageElement> battCar;
	std::unique_ptr<ImageElement> ctrl;
	std::unique_ptr<ImageElement> battCtrl;
	std::unique_ptr<ImageElement> signal;

public:
	void draw(Sprite* canvas) override;

};


#endif //NEVERA_CONTROLLER_BAR_H
