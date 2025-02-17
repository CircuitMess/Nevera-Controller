#ifndef NEVERA_CONTROLLER_SCREEN_H
#define NEVERA_CONTROLLER_SCREEN_H

#include <Util/StateMachine/State.h>
#include <Devices/Display.h>
#include "UI/Element.h"
#include <unordered_set>

class Screen : public State {
	GENERATED_BODY(Screen, State);

public:
	~Screen() override;

	const Class* transitionTo() const noexcept override final;
	void tick(float deltaTime) noexcept override final;

protected:
	void transition(const Class* next) noexcept;

	void addElement(Element* el);
	void removeElement(Element* el);

private:
	const Class* nextScreen = nullptr;

	virtual void update();
	virtual void preRender(Sprite* canvas);
	virtual void postRender(Sprite* canvas);

	void render();
	std::vector<Element*> elements;

	static constexpr uint32_t FrameTime = 25;

};

#endif //NEVERA_CONTROLLER_SCREEN_H
