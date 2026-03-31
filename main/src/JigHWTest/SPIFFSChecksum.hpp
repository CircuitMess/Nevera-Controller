#ifndef SPIFFSCHECKSUM_HPP
#define SPIFFSCHECKSUM_HPP

struct {
	const char* name;
	uint32_t sum;
} static const SPIFFSChecksums[] = {
	{ "/spiffs/Drive/Batt0.raw", 14859},
	{ "/spiffs/Drive/Batt1.raw", 15074},
	{ "/spiffs/Drive/Batt2.raw", 15813},
	{ "/spiffs/Drive/Batt3.raw", 16290},
	{ "/spiffs/Drive/Batt4.raw", 16767},
	{ "/spiffs/Drive/Batt5.raw", 17244},
	{ "/spiffs/Drive/Batt6.raw", 17721},
	{ "/spiffs/Drive/Batt7.raw", 18198},
	{ "/spiffs/Drive/Car.raw", 15417},
	{ "/spiffs/Drive/Ctrl.raw", 22803},
	{ "/spiffs/Drive/Signal0.raw", 2580},
	{ "/spiffs/Drive/Signal1.raw", 2984},
	{ "/spiffs/Drive/Signal2.raw", 5004},
	{ "/spiffs/Drive/Signal3.raw", 8640},
	{ "/spiffs/Drive/SpeedBar.raw", 27876},
	{ "/spiffs/Intro.gif", 2565583},
	{ "/spiffs/Pair/Arrow.raw", 6414},
	{ "/spiffs/Pair/Bg.raw", 606739},
	{ "/spiffs/Pair/In.gif", 1035930},
	{ "/spiffs/Pair/Out.gif", 1363750},
};

#endif //SPIFFSCHECKSUM_HPP
