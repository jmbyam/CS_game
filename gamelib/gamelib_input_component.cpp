#include "pch.h"
#include <gamelib_random.hpp>
#include <gamelib_locator.hpp>
#include <gamelib_input_component.hpp>

namespace GameLib {
    void SimpleInputComponent::update(Actor& actor) {
        auto axis1 = Locator::getInput()->axis1X;
        if (axis1)
            actor.velocity.x = axis1->getAmount();
        auto axis2 = Locator::getInput()->axis1Y;
        if (axis2)
            actor.velocity.y = axis2->getAmount();
    }

    void RandomInputComponent::update(Actor& actor) {
        if (count++ > 5) {
            count = 0;
        } else {
            return;
        }
        actor.velocity.x = random.normal();
        actor.velocity.y = random.normal();
        glm::normalize(actor.velocity);
    }
}
