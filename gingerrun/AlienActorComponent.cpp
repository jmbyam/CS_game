#include "AlienActorComponent.hpp"

void AlienActorComponent::update(GameLib::Actor& actor, GameLib::World& world) {
    // do actor stuff here
    float t = GameLib::Context::currentTime_s;
    int frame = (int)t;

    actor.velocity.y += actor.dt * 9.0f;

	if (frame % 15 == 0) {
        animTime = t + 1;
        state = State::Exploding;
    }

    if (frame % 15 == 5) {
        animTime = t + 1;
        state = State::Beaming;
    }

	if (animTime > t) {
        frame = GameLib::clamp<int>(8 - (int)(8 * (animTime - t)), 0, 7);
    } else {
        state = State::Normal;
        frame = int(8 * t) % 8;
    }

    switch (state) {
    case State::Normal:
        actor.spriteId = 24 + frame;
        break;
    case State::Exploding:
        actor.spriteId = 8 + frame;
        break;
    case State::Beaming:
        actor.spriteId = 16 + frame;
        break;
    }
}
