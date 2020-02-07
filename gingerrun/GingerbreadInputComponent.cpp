#include <gamelib_locator.hpp>
#include "GingerbreadInputComponent.hpp"

void GingerbreadInputComponent::update(GameLib::Actor& actor) {
    float vx = 1;
    float h = 8;
    float x = 4;
    float v0 = 2 * h * vx / x;

	auto axis1 = GameLib::Locator::getInput()->axis1X;
    if (axis1)
        actor.velocity.x = vx * axis1->getAmount();

    if (lastY == actor.position.y) {
        auto axis2 = GameLib::Locator::getInput()->axis1Y;

        if (axis2)
            actor.velocity.y = v0 * axis2->getAmount();
    }

    lastY = actor.position.y;

    if (actor.spriteFlipX && actor.velocity.x > 0)
        actor.spriteFlipX = false;
    if (!actor.spriteFlipX && actor.velocity.x < 0)
        actor.spriteFlipX = true;
}
