#include "Element.h"

int16_t Element::getX() const{
	return x;
}

void Element::setX(int16_t x){
	Element::x = x;
}

int16_t Element::getY() const{
	return y;
}

void Element::setY(int16_t y){
	Element::y = y;
}

void Element::setPos(int16_t x, int16_t y){
	Element::x = x;
	Element::y = y;
}
