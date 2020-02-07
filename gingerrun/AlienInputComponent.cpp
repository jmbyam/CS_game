#include "AlienInputComponent.hpp"

void AlienInputComponent::update(GameLib::Actor& actor) {
    if (counter_ < GameLib::Context::currentTime_s) {
        counter_ = GameLib::Context::currentTime_s + 1.0f;
        actor.velocity.x = GameLib::random.normal();
        actor.velocity.y = GameLib::random.normal();
        glm::normalize(actor.velocity);
    }
}
