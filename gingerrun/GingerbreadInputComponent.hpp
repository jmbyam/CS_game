#pragma once
#include <gamelib_input_component.hpp>

class GingerbreadInputComponent : public GameLib::InputComponent {
public:
    virtual ~GingerbreadInputComponent() {}

	void update(GameLib::Actor& actor) override;

private:
    float lastY{ 0 };
};
