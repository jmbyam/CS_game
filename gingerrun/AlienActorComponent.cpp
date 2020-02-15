#include "AlienActorComponent.hpp"

void AlienActorComponent::beginPlay(GameLib::Actor& a) {
	a.anims.resize((int)State::MaxStates);
	a.anims[(int)State::Normal].reset(24, 8, 15.0f, GameLib::Actor::ANIMINFO::CYCLE);
	a.anims[(int)State::Beaming].reset(8, 8, 15.0f, GameLib::Actor::ANIMINFO::CYCLE);
	a.anims[(int)State::Exploding].reset(16, 8, 15.0f, GameLib::Actor::ANIMINFO::CYCLE);
}

void AlienActorComponent::update(GameLib::Actor& actor, GameLib::World& world) {
	// do actor stuff here
	float t = GameLib::Context::currentTime_s;
	int frame = (int)t;

	actor.velocity.y += actor.dt * 9.0f;

	if (frame % 15 == 0) {
		state = State::Exploding;
		actor.switchAnim((int)State::Exploding);
	}

	if (frame % 15 == 5) {
		animTime = t + 1;
		state = State::Beaming;
		actor.switchAnim((int)State::Beaming);
	}

	if (state != State::Normal && actor.anim.ended()) {
		state = State::Normal;
		actor.switchAnim((int)State::Normal);
	}
}
