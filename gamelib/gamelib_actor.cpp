#include "pch.h"
#include <gamelib_actor.hpp>
#include <gamelib_locator.hpp>

namespace GameLib {
	unsigned Actor::idSource_{0};

	Actor::Actor(InputComponent* input, ActorComponent* actor, PhysicsComponent* physics, GraphicsComponent* graphics)
		: transform(1.0f), addlTransform(1.0f), input_(input), actor_(actor), physics_(physics), graphics_(graphics) {
		id_ = idSource_++;
	}

	Actor::~Actor() {
		delete input_;
		delete physics_;
		delete graphics_;
	}

	void Actor::beginPlay() {
		if (actor_)
			actor_->beginPlay(*this);
	}

	void Actor::update(float deltaTime, World& world) {
		dt = deltaTime;
		if (input_)
			input_->update(*this);
		if (actor_)
			actor_->update(*this, world);
	}

	void Actor::physics(float deltaTime, World& world) {
		if (!physics_)
			return;
		lastPosition = position;
		physics_->update(*this, world);
		if (actor_) {
			if (physics_->collideWorld(*this, world))
				actor_->handleCollisionWorld(*this, world);

			for (Actor* b : world.staticActors) {
				if (this == b)
					continue;
				if (physics_->collideStatic(*this, *b))
					actor_->handleCollisionStatic(*this, *b);
			}

			for (Actor* b : world.dynamicActors) {
				if (this == b)
					continue;
				if (physics_->collideDynamic(*this, *b))
					actor_->handleCollisionDynamic(*this, *b);
			}

			if (triggerInfo.overlapping && triggerInfo.triggerActor) {
				Actor* trigger = triggerInfo.triggerActor;
				if (!physics_->collideTrigger(*this, *trigger)) {
					triggerInfo.overlapping = false;
					triggerInfo.triggerActor = nullptr;
					actor_->endOverlap(*this, *trigger);

					if (trigger->actor_) {
						trigger->triggerInfo.overlapping = false;
						trigger->actor_->endTriggerOverlap(*trigger, *this);
					}
				}
			} else {
				for (Actor* trigger : world.triggerActors) {
					if (this == trigger)
						continue;
					if (!triggerInfo.overlapping && physics_->collideTrigger(*this, *trigger)) {
						triggerInfo.overlapping = true;
						actor_->beginOverlap(*this, *trigger);
						if (trigger->actor_) {
							trigger->triggerInfo.overlapping = true;
							trigger->actor_->beginTriggerOverlap(*trigger, *this);
							triggerInfo.triggerActor = trigger;
						}
					}
				}
			}
		}
		dPosition = position - lastPosition;
	}

	void Actor::draw(Graphics& graphics) {
		if (visible && graphics_)
			graphics_->draw(*this, graphics);
	}

	void Actor::makeDynamic() {
		type_ = DYNAMIC;
	}

	void Actor::makeStatic() {
		type_ = STATIC;
	}

	void Actor::makeTrigger() {
		type_ = TRIGGER;
	}
} // namespace GameLib
