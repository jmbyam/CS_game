#pragma once
#include <gamelib.hpp>

class AlienInputComponent : public GameLib::InputComponent {
public:
    virtual ~AlienInputComponent() {}

    void update(GameLib::Actor& actor) override;

private:
    float counter_{ 0 };
};
