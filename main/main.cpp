#include <Core/EntryPoint.h>
#include <FileSystem/SPIFFS.h>
#include "Periphery/GPIOPeriph.h"
#include <Drivers/Interface/InputDriver.h>
#include <Drivers/Input/InputGPIO.h>
#include <Periphery/I2C.h>
#include <Devices/AW9523.h>
#include <Drivers/Output/OutputCurrAW.h>
#include <Services/ButtonInput.h>
#include <Misc/Enum.h>
#include <Periphery/WiFi.h>
#include <Util/StateMachine/StateMachine.h>
#include <Devices/Display.h>
#include "src/Pins.hpp"
#include "src/HardwareConfiguration.h"
#include "src/Services/WiFiAccessPoint.h"
#include "src/Services/TCPServer.h"
#include "src/Screens/IntroScreen.h"
#include "src/Services/UDPListener.h"
#include <nvs_flash.h>
#include "Services/Comm.h"
#include <Services/LED/LED.h>
#include <Services/LED/LEDFadeFunction.h>
#include <Services/LED/LEDBlinkFunction.h>

DECLARE_ENUM(Button, Up, Down, Left, Right, Menu, Forward, Backward);

DECLARE_ENUM(LEDs, Slider0, Slider1, Slider2, Slider3, Slider4, Boost0, Boost1, BatteryFull, BatteryLow, Backlight);
DECLARE_ENUM(RGB_LEDs);

class NeveraController : public Application {
	GENERATED_BODY(NeveraController, Application)

protected:
	virtual void begin() noexcept override {
		Super::begin();

		auto ret = nvs_flash_init();
		if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND){
			ESP_ERROR_CHECK(nvs_flash_erase());
			ret = nvs_flash_init();
		}
		ESP_ERROR_CHECK(ret);

		HardwareConfiguration* config = registerSingleton<HardwareConfiguration>();

		GPIOPeriph* gpio = registerPeriphery<GPIOPeriph>();
		InputGPIO* inputGPIO = registerDriver<InputGPIO>(config->getGPIOInputs(), gpio);

		static const std::vector<std::pair<Enum<int>, InputPin>> ButtonInputs = {
			{ Button::Up, { inputGPIO, BTN_UP }},
			{ Button::Down, { inputGPIO, BTN_DOWN }},
			{ Button::Left, { inputGPIO, BTN_LEFT }},
			{ Button::Right, { inputGPIO, BTN_RIGHT }},
			{ Button::Forward, { inputGPIO, BTN_FWD }},
			{ Button::Backward, { inputGPIO, BTN_BCK }},
			{ Button::Menu, { inputGPIO, BTN_MENU }}
		};

		ButtonInput* buttonInput = registerService<ButtonInput>();
		buttonInput->reg(ButtonInputs);

		I2C* i2c = registerPeriphery<I2C>(I2CPort::Zero, static_cast<gpio_num_t>(I2C_SDA), static_cast<gpio_num_t>(I2C_SCL));

		AW9523* aw9523 = registerDevice<AW9523>(i2c, config->getAW9523Address());
		aw9523->setCurrentLimit(AW9523::IMAX);


		OutputCurrAW* outputCurrAW = registerService<OutputCurrAW>(config->getAW9523Outputs(), aw9523);
		for(const auto out: config->getAW9523Outputs()){
			outputCurrAW->write(out.port, false);
		}

		outputCurrAW->write(LED_BACKLIGHT, true);


		Display* display = registerDevice<Display>(config->getDisplayBusConfig(), config->getDisplayPanelConfig(), [](Sprite& canvas){
							canvas.setColorDepth(lgfx::rgb565_2Byte);
							canvas.createSprite(128, 128);
						});
		display->getLGFX().setSwapBytes(true);
		display->drawTest();


		static const std::vector<std::pair<LEDs, OutputPin>> ledPins = {
				{ LEDs::Slider0,     { outputCurrAW, LED_SLIDER0 }},
				{ LEDs::Slider1,     { outputCurrAW, LED_SLIDER1 }},
				{ LEDs::Slider2,     { outputCurrAW, LED_SLIDER2 }},
				{ LEDs::Slider3,     { outputCurrAW, LED_SLIDER3 }},
				{ LEDs::Slider4,     { outputCurrAW, LED_SLIDER4 }},
				{ LEDs::Boost0,      { outputCurrAW, LED_BOOST0 }},
				{ LEDs::Boost1,      { outputCurrAW, LED_BOOST1 }},
				{ LEDs::BatteryLow,  { outputCurrAW, LED_BATTLOW }},
				{ LEDs::BatteryFull, { outputCurrAW, LED_BATTFULL }}
		};
		LED<LEDs, RGB_LEDs>* ledService = registerService<LED<LEDs, RGB_LEDs>>();
		ledService->reg(ledPins);

		WiFi* wifi = registerPeriphery<WiFi>();
		wifi->setHidden(true);

		registerService<WiFiAccessPoint>();
		registerService<TCPServer>();
		registerService<Comm>();

		/*static const std::map<Enum<int>, lv_key_t> LVGLMappings = {
			{ Button::Up, LV_KEY_UP },
			{ Button::Down, LV_KEY_DOWN },
			{ Button::Left, LV_KEY_LEFT },
			{ Button::Right, LV_KEY_RIGHT },
			{ Button::Forward, LV_KEY_NEXT },
			{ Button::Backward, LV_KEY_PREV },
			{ Button::Menu, LV_KEY_HOME },
		};*/

		// TODO this causes a corrupt heap error for some reason
		// inputLvgl = newObject<InputLVGL>(this, buttonInput, LVGLMappings);

		/*lvgl = newObject<LVGL>(this, display, [this](lv_disp_t* disp) -> lv_theme_t*{
			// Init a theme
			return lv_theme_simple_init(disp);
		});*/

		if(!SPIFFS::init()) {
			return;
		}
	}

	virtual void tick(float deltaTime) noexcept override {
		Super::tick(deltaTime);
	}

	virtual void onDestroy() noexcept override {
		Super::onDestroy();
	}
};

CMF_MAIN(NeveraController)