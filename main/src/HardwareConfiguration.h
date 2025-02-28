#ifndef NEVERA_HARDWARECONFIGURATION_H
#define NEVERA_HARDWARECONFIGURATION_H

#include <lgfx/v1/panel/Panel_Device.hpp>
#include <lgfx/v1/platforms/esp32/Bus_SPI.hpp>
#include <Misc/Singleton.h>
#include "Pins.hpp"
#include <Drivers/Input/InputTouchGPIO.h>

class HardwareConfiguration : public Singleton {
    GENERATED_BODY(HardwareConfiguration, Singleton)

public:
	const std::vector<GPIOPinDef>& getGPIOInputs() const noexcept { return GPIOInputs; }
	const std::vector<OutputPinDef>& getGPIOOutputs() const noexcept { return GPIOOutputs; }
	uint8_t getAW9523Address() const noexcept { return AW9523Address; }
	const std::vector<OutputPinDef>& getAW9523Outputs() const noexcept { return AW9523Outputs; }
	const lgfx::Bus_SPI::config_t& getDisplayBusConfig() const noexcept { return DisplayBusConfig; }
	const lgfx::Panel_Device::config_t& getDisplayPanelConfig() const noexcept { return DisplayPanelConfig; }

	const std::vector<TouchPinDef>& getTouchInputs() const{ return TouchInputs; }

private:
    const std::vector<GPIOPinDef> GPIOInputs = {
		{ { BTN_UP, true }, PullMode::Up },
		{ { BTN_DOWN, true }, PullMode::Up },
		{ { BTN_LEFT, true }, PullMode::Up },
		{ { BTN_RIGHT, true }, PullMode::Up },
		{ { BTN_FWD, true }, PullMode::Up },
		{ { BTN_BCK, true }, PullMode::Up },
		{ { BTN_MENU, true }, PullMode::Up }
	};

	const std::vector<OutputPinDef> GPIOOutputs = {
			{PIN_VREF, false}
	};

	const uint8_t AW9523Address = 0x5b;

	//ports are not inverted since AW9523 led driver is a current source
	const std::vector<OutputPinDef> AW9523Outputs = {
			{ LED_SLIDER0,   false },
			{ LED_BOOST0,    false },
			{ LED_BOOST1,    false },
			{ LED_SLIDER4,   false },
			{ LED_SLIDER3,   false },
			{ LED_SLIDER2,   false },
			{ LED_SLIDER1,   false },
			{ LED_BATTLOW,   false },
			{ LED_BATTFULL,  false },
			{ LED_BACKLIGHT, false }
	};

	const lgfx::Bus_SPI::config_t DisplayBusConfig = {
		.freq_write = 40000000,
		.freq_read = 40000000,
		.pin_sclk = TFT_SCK,
		.pin_miso = -1,
		.pin_mosi = TFT_SDA,
		.pin_dc = TFT_DC,
		.spi_mode = 0,
		.spi_3wire = false,
		.use_lock = false,
		.dma_channel = LGFX_ESP32_SPI_DMA_CH,
		.spi_host = SPI2_HOST
	};

	const lgfx::Panel_Device::config_t DisplayPanelConfig = {
		.pin_cs = -1,
		.pin_rst = TFT_RST,
		.pin_busy = -1,
		.memory_width = 132,
		.memory_height = 132,
		.panel_width = 128,
		.panel_height = 128,
		.offset_x = 2,
		.offset_y = 1,
		.offset_rotation = 3,
		.readable = false,
		.invert = false,
		.rgb_order = false,
		.dlen_16bit = false,
		.bus_shared = false
	};


	/**
	 * Empirically determined.
	 * Value should be as low as possible, but such that touch from the opposite side of the PCB isn't registered.
	 */
	static constexpr uint32_t TouchThreshold = 5000;
	const std::vector<TouchPinDef> TouchInputs = {
			{{SLIDER_0, false}, TouchThreshold},
			{{SLIDER_1, false}, TouchThreshold},
			{{SLIDER_2, false}, TouchThreshold},
			{{SLIDER_3, false}, TouchThreshold},
			{{SLIDER_4, false}, TouchThreshold}
	};
};

#endif //NEVERA_HARDWARECONFIGURATION_H