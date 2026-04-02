#include <iostream>
#include <Core/EntryPoint.h>
#include <FileSystem/SPIFFS.h>
#include "Periphery/GPIOPeriph.h"
#include <Drivers/Interface/InputDriver.h>
#include <Drivers/Input/InputGPIO.h>
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
#include <Drivers/Output/OutputGPIO.h>
#include "src/Services/Battery.h"
#include <nvs_flash.h>
#include "Services/Comm.h"
#include <Services/LED/LED.h>
#include <Services/LED/LEDFadeFunction.h>
#include <Services/LED/LEDBlinkFunction.h>
#include "Enums.h"
#include "Drivers/Input/InputTouchGPIO.h"
#include <Drivers/Output/OutputPWM.h>
#include <Periphery/I2CMaster.h>
#include <Services/Feed.h>
#include "Services/BatteryIndicator.h"
#include "Services/ShutdownService.h"
#include "Util/EfuseMeta.h"
#include "JigHWTest/JigHWTest.h"

class NeveraController : public Application {
	GENERATED_BODY(NeveraController, Application, void)

public:
	NeveraController() noexcept : Super(1000, 6 * 1024, 8, 1) {}

protected:
	virtual void begin() noexcept override {
		auto ret = nvs_flash_init();
		if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND){
			ESP_ERROR_CHECK(nvs_flash_erase());
			ret = nvs_flash_init();
		}
		ESP_ERROR_CHECK(ret);

		HardwareConfiguration* config = registerSingleton<HardwareConfiguration>();

		if(JigHWTest::checkJig()){
			printf("Jig\n");
			StrongObjectPtr<JigHWTest> test = newObject<JigHWTest>();
			test->start();
			vTaskDelete(nullptr);
		}else{
			printf("Hello\n");
		}

		if(!EfuseMeta::check()){
			while(true){
				vTaskDelay(1000);
				EfuseMeta::log();
			}
		}

		GPIOPeriph* gpio = registerPeriphery<GPIOPeriph>();
		InputGPIO* inputGPIO = registerDriver<InputGPIO>(config->getGPIOInputs(), gpio);
		OutputGPIO* outputGPIO = registerDriver<OutputGPIO>(config->getGPIOOutputs(), gpio);

		auto touchInput = registerDriver<InputTouchGPIO>(config->getTouchInputs());

		static const std::vector<std::pair<Enum<int>, InputPin>> ButtonInputs = {
				{ Button::Up,       { inputGPIO,  BTN_UP }},
				{ Button::Down,     { inputGPIO,  BTN_DOWN }},
				{ Button::Left,     { inputGPIO,  BTN_LEFT }},
				{ Button::Right,    { inputGPIO,  BTN_RIGHT }},
				{ Button::Forward,  { inputGPIO,  BTN_FWD }},
				{ Button::Backward, { inputGPIO,  BTN_BCK }},
				{ Button::Menu,     { inputGPIO,  BTN_MENU }},
				{ Button::Slider0,  { touchInput, SLIDER_0 }},
				{ Button::Slider1,  { touchInput, SLIDER_1 }},
				{ Button::Slider2,  { touchInput, SLIDER_2 }},
				{ Button::Slider3,  { touchInput, SLIDER_3 }},
				{ Button::Slider4,  { touchInput, SLIDER_4 }}
		};

		ButtonInput* buttonInput = registerService<ButtonInput>();
		buttonInput->reg(ButtonInputs);

		I2CMaster*  i2cMaster = registerPeriphery<I2CMaster>(I2CPort::Zero, static_cast<gpio_num_t>(I2C_SDA), static_cast<gpio_num_t>(I2C_SCL));

		AW9523* aw9523 = registerDevice<AW9523>(i2cMaster, config->getAW9523Address());
		aw9523->setCurrentLimit(AW9523::IMAX);

		OutputCurrAW* outputCurrAW = registerService<OutputCurrAW>(config->getAW9523Outputs(), aw9523);
		for(const auto out: config->getAW9523Outputs()){
			outputCurrAW->write(out.port, false);
		}

		OutputPWM* outputPWM = registerDriver<OutputPWM>(config->getPwmOutputs());


		static const std::vector<std::pair<LEDs, OutputPin>> ledPins = {
				{ LEDs::Slider0,     { outputCurrAW, LED_SLIDER0 }},
				{ LEDs::Slider1,     { outputCurrAW, LED_SLIDER1 }},
				{ LEDs::Slider2,     { outputCurrAW, LED_SLIDER2 }},
				{ LEDs::Slider3,     { outputCurrAW, LED_SLIDER3 }},
				{ LEDs::Slider4,     { outputCurrAW, LED_SLIDER4 }},
				{ LEDs::Boost0,      { outputCurrAW, LED_BOOST0 }},
				{ LEDs::Boost1,      { outputCurrAW, LED_BOOST1 }},
				{ LEDs::BatteryLow,  { outputCurrAW, LED_BATTLOW }},
				{ LEDs::BatteryFull, { outputCurrAW, LED_BATTFULL }},
				{ LEDs::Backlight,   { outputPWM, LED_BACKLIGHT }},
				{ LEDs::Power,       { outputPWM, LED_PWR }}
		};
		LED<LEDs, RGB_LEDs>* ledService = registerService<LED<LEDs, RGB_LEDs>>();
		ledService->reg(ledPins);

		ledService->on(LEDs::Power, 0.03f);

		registerPeriphery<WiFi>();
		registerService<WiFiAccessPoint>();
		registerService<TCPServer>();
		registerService<UDPListener>();
		registerService<Comm>();

		registerService<ShutdownService>();

		Battery* battery = registerService<Battery>(OutputPin{ outputGPIO, PIN_VREF });
		battery->OnLevelChanged.bind(this, &NeveraController::onBatteryChange);
		battery->begin();

		onBatteryChange(battery->getLevel());

		if(!SPIFFS::init()) {
			return;
		}

		ledService->on(LEDs::Backlight, 1.0f);

		Display* display = registerDevice<Display>(config->getDisplayBusConfig(), config->getDisplayPanelConfig(), [](Sprite& canvas){
							canvas.setColorDepth(lgfx::rgb565_2Byte);
							canvas.createSprite(128, 128);
						});
		display->getLGFX().setSwapBytes(true);

		registerService<BatteryIndicator>(battery, ledService);

		registerService<Feed>();

		StateMachine* stateMachine = registerService<StateMachine>(26, 8 * 1024, 6, 0);
		stateMachine->setStartingStateType(IntroScreen::staticClass());
	}

private:
	void onBatteryChange(Battery::Level level) {
		if(level != Battery::Level::Critical) {
			return;
		}

		ShutdownService::shutdown(ShutdownReason::Battery);
		vTaskDelay(portMAX_DELAY);
	}
};

CMF_MAIN(NeveraController)