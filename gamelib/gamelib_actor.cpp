#include "pch.h"
#include <gamelib_actor.hpp>
#include <gamelib_locator.hpp>

namespace GameLib {
    unsigned Actor::idSource_{ 0 };

    Actor::Actor(InputComponent* input, ActorComponent* actor, PhysicsComponent* physics, GraphicsComponent* graphics)
        : transform(1.0f)
        , addlTransform(1.0f)
        , input_(input)
        , actor_(actor)
        , physics_(physics)
        , graphics_(graphics) {
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
        physics_->update(*this, world);
        if (actor_) {
            if (physics_->collideWorld(*this, world))
                actor_->handleCollisionWorld(*this, world);

            for (Actor* b : world.staticActors) {
                if (physics_->collideDynamic(*this, *b))
                    actor_->handleCollisionStatic(*this, *b);
            }

            for (Actor* b : world.dynamicActors) {
                if (physics_->collideDynamic(*this, *b))
                    actor_->handleCollisionDynamic(*this, *b);
            }

            for (Actor* trigger : world.triggerActors) {
                if (!triggerInfo.overlapping && physics_->collideTrigger(*this, *trigger)) {
                    triggerInfo.overlapping = true;
                    actor_->beginOverlap(*this, *trigger);
                } else if (triggerInfo.overlapping && !physics_->collideTrigger(*this, *trigger)) {
                    triggerInfo.overlapping = false;
                    actor_->endOverlap(*this, *trigger);
                }
            }
        }
    }

    void Actor::draw(Graphics& graphics) {
        if (visible && graphics_)
            graphics_->draw(*this, graphics);
    }
}
