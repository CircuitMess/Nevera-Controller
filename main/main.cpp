#include <Core/EntryPoint.h>
#include <FileSystem/SPIFFS.h>
#include <LV_Interface/InputLVGL.h>
#include <LV_Interface/LVGL.h>
#include <LV_Interface/FSLVGL.h>
#include "Periphery/GPIOPeriph.h"
#include <Drivers/Interface/InputDriver.h>
#include <Drivers/Input/InputGPIO.h>
#include <Periphery/I2C.h>
#include <Devices/AW9523.h>
#include <Drivers/Output/OutputDigAW.h>
#include <Services/ButtonInput.h>
#include <Misc/Enum.h>
#include <Periphery/WiFi.h>
#include "src/Pins.hpp"
#include "src/HardwareConfiguration.h"
#include "src/Services/WiFiAccessPoint.h"

DECLARE_ENUM(Button, Up, Down, Left, Right, Menu, Forward, Backward);

class NeveraController : public Application {
	GENERATED_BODY(NeveraController, Application)

protected:
	virtual void begin() noexcept override {
		Super::begin();

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

		OutputDigAW* outputDigAW = registerService<OutputDigAW>(config->getAW9523Outputs(), aw9523);
		outputDigAW->performWrite(15, 1.0f);

		Display* display = registerDevice<Display>(config->getDisplayBusConfig(), config->getDisplayPanelConfig(), [](Sprite& canvas){
							canvas.setColorDepth(lgfx::rgb565_2Byte);
							canvas.createSprite(128, 128);
						});
		display->getLGFX().setSwapBytes(true);
		display->drawTest();

		WiFi* wifi = registerPeriphery<WiFi>();
		wifi->setHidden(true);

		registerService<WiFiAccessPoint>();

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

private:
	StrongObjectPtr<LVGL> lvgl;
	StrongObjectPtr<InputLVGL> inputLvgl;
	StrongObjectPtr<FSLVGL> fslvgl;
};

CMF_MAIN(NeveraController)