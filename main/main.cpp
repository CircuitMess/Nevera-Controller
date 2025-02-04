#include <Core/EntryPoint.h>

class TestApp : public Application {
	GENERATED_BODY(TestApp, Application)

protected:
	virtual void begin() noexcept override {
		Super::begin();
	}

	virtual void tick(float deltaTime) noexcept override {
		Super::tick(deltaTime);
	}

	virtual void onDestroy() noexcept override {
		Super::onDestroy();
	}
};

CMF_MAIN(TestApp)