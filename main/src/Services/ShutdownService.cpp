#include "ShutdownService.h"
#include "Pins.hpp"
#include "Enums.h"
#include "Drivers/Output/OutputGPIO.h"
#include "Services/LED/LED.h"

SemaphoreHandle_t ShutdownService::shutdownSem = xSemaphoreCreateBinary();

ShutdownService::ShutdownService() : Super(Timeout, 3 * 1024, 10, -1){
	inactivitySem = xSemaphoreCreateBinary();
	xSemaphoreGive(inactivitySem);

	xSemaphoreGive(shutdownSem);

	auto buttonInput = getApp()->getService<ButtonInput>();
	buttonInput->OnButtonEvent.bind(this, &ShutdownService::inputEvent);
}

void ShutdownService::shutdown(ShutdownReason reason){
	xSemaphoreTake(shutdownSem, portMAX_DELAY);
	auto app = getApp();

	auto leds = app->getService<LED<LEDs, RGB_LEDs>>();

	for(int i = 0; i < (uint8_t) LEDs::COUNT; i++){
		leds->off((LEDs) i);
	}

	leds->forceUpdate();

	gpio_reset_pin(static_cast<gpio_num_t>(LED_BACKLIGHT));
	gpio_set_direction(static_cast<gpio_num_t>(LED_BACKLIGHT), GPIO_MODE_OUTPUT);
	gpio_set_level(static_cast<gpio_num_t>(LED_BACKLIGHT), true);
	gpio_hold_en(static_cast<gpio_num_t>(LED_BACKLIGHT));

	gpio_deep_sleep_hold_en();

	ESP_ERROR_CHECK(esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_AUTO));
	ESP_ERROR_CHECK(esp_sleep_pd_config(ESP_PD_DOMAIN_RC_FAST, ESP_PD_OPTION_AUTO));
	ESP_ERROR_CHECK(esp_sleep_pd_config(ESP_PD_DOMAIN_CPU, ESP_PD_OPTION_AUTO));
	ESP_ERROR_CHECK(esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_AUTO));
	ESP_ERROR_CHECK(esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL));
	esp_deep_sleep_start();
}

void ShutdownService::tick(float deltaTime) noexcept{
	Super::tick(deltaTime);

	if(xSemaphoreTake(inactivitySem, 0) == pdFALSE){
		//Semaphore not given in the last 'Timeout' milliseconds by the inputEvent callback.
		shutdown(ShutdownReason::Inactivity);
	}
}

void ShutdownService::inputEvent(Enum<int> btn, ButtonInput::Action action){
	xSemaphoreGive(inactivitySem);
}
