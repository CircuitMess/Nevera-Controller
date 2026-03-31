#ifndef CLOCKSTAR_FIRMWARE_JIGHWTEST_H
#define CLOCKSTAR_FIRMWARE_JIGHWTEST_H

#include <vector>
#include "Devices/Display.h"
#include "Util/stdafx.h"
#include "Services/Battery.h"
#include <esp_efuse.h>
#include <esp_spiffs.h>
#include "HardwareConfiguration.h"
#include "Periphery/I2CMaster.h"
#include "Periphery/I2CDevice.h"
#include <Pins.hpp>

struct Test {
	bool (* test)();
	const char* name;
	void (* onFail)();
};

class JigHWTest : public Object{
	GENERATED_BODY(JigHWTest, Object, void)

public:
	JigHWTest();
	static bool checkJig();
	void start();

private:
	inline static StrongObjectPtr<HardwareConfiguration> config = nullptr;
	inline static StrongObjectPtr<Display> display = nullptr;
	inline static LGFX_Sprite* canvas = nullptr;

	inline static StrongObjectPtr<I2CMaster> i2cMaster = nullptr;
	inline static JigHWTest* test = nullptr;

	std::vector<Test> tests;
	const char* currentTest;

	void log(const char* property, const char* value) const;
	void log(const char* property, float value) const;
	void log(const char* property, double value) const;
	void log(const char* property, bool value) const;
	void log(const char* property, uint32_t value) const;
	void log(const char* property, int32_t value) const;
	void log(const char* property, const std::string& value) const;

	static bool AW9523Check();
	static bool BatteryCheck();
	static bool VoltReferenceCheck();
	static bool SPIFFSTest();
	static uint32_t calcChecksum(FILE* file);
	static bool HWVersion();

	static constexpr int16_t BatVoltageMinimum = 3300;
	static constexpr float VoltReference = 2500;
	static constexpr float VoltReferenceTolerance = 100;

	static constexpr uint32_t CheckTimeout = 500;

	static constexpr esp_vfs_spiffs_conf_t spiffsConfig = {
			.base_path = "/spiffs",
			.partition_label = "storage",
			.max_files = 8,
			.format_if_mount_failed = false
	};

	static constexpr uint8_t ButtonCount = 4;
};

#endif //CLOCKSTAR_FIRMWARE_JIGHWTEST_H
