#pragma once
#include <gamelib.hpp>

class AlienActorComponent : public GameLib::ActorComponent {
public:
	virtual ~AlienActorComponent() {}

	void beginPlay(GameLib::Actor& actor) override;
	void update(GameLib::Actor& actor, GameLib::World& world) override;

private:
	enum class State { Normal, Exploding, Beaming, MaxStates };
	State state{ State::Normal };
	float animTime;
};
