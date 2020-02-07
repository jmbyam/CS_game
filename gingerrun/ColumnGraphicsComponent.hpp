#pragma once
#include <gamelib.hpp>

class ColumnGraphicsComponent : public GameLib::GraphicsComponent {
public:
    virtual ~ColumnGraphicsComponent() {}

	void draw(GameLib::Actor& actor, GameLib::Graphics& graphics) override;
};
