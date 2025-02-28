#ifndef NEVERA_CONTROLLER_PINS_HPP
#define NEVERA_CONTROLLER_PINS_HPP

#define I2C_SDA 12
#define I2C_SCL 13

//AW9523 LEDs
#define LED_SLIDER0 0
#define LED_BOOST0 1
#define LED_BOOST1 2
#define LED_SLIDER4 8
#define LED_SLIDER3 9
#define LED_SLIDER2 10
#define LED_SLIDER1 11
#define LED_BATTFULL 13
#define LED_BATTLOW 14
#define LED_BACKLIGHT 15

#define PIN_BATT 10
#define PIN_VREF 35

#define TFT_SCK 47
#define TFT_SDA 33
#define TFT_DC 34
#define TFT_RST 48

#define BTN_FWD 15
#define BTN_BCK 16
#define BTN_UP 37
#define BTN_DOWN 39
#define BTN_LEFT 38
#define BTN_RIGHT 36
#define BTN_MENU 40

//Slider touchpads are indexed the same as GPIOs
#define SLIDER_0 2
#define SLIDER_1 4
#define SLIDER_2 5
#define SLIDER_3 6
#define SLIDER_4 7

#endif //NEVERA_CONTROLLER_PINS_HPP