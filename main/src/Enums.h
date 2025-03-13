#ifndef ENUM_H
#define ENUM_H

#include <Misc/Enum.h>

DECLARE_ENUM(Button, Up, Down, Left, Right, Menu, Forward, Backward, Slider0, Slider1, Slider2, Slider3, Slider4);

enum class LEDs {
	Slider0, Slider1, Slider2, Slider3, Slider4, Boost0, Boost1, BatteryFull, BatteryLow, Backlight, Power, COUNT
};
enum class RGB_LEDs {
};

#endif //ENUM_H
