#ifndef GAMELIB_ACTOR_COMPONENT_HPP
#define GAMELIB_ACTOR_COMPONENT_HPP

#include <gamelib_graphics.hpp>
#include <gamelib_actor.hpp>
#include <gamelib_world.hpp>

namespace GameLib {
    class ActorComponent {
    public:
        virtual ~ActorComponent() {}
        // update() is called to update internal game logic
        virtual void update(Actor& actor, World& world) {}
        // beginPlay() is called when actor is added to the game world
        virtual void beginPlay(Actor& actor) {}
        // handleCollisionStatic() is called when an actor interacts with a dynamic actor
        virtual void handleCollisionStatic(Actor& a, Actor& b) {}
        // handleCollisionDynamic() is called when an actor interacts with a dynamic actor
        virtual void handleCollisionDynamic(Actor& a, Actor& b) {}
        // handleCollisionWorld() is called when an actor has a collision in the game world
        virtual void handleCollisionWorld(Actor& actor, World& world) {}
        // beginOverlap() is called when the actor collides with a trigger
        virtual void beginOverlap(Actor& a, Actor& b) {}
        // endOverlap() is called when the actor stops colliding with a trigger
        virtual void endOverlap(Actor& a, Actor& b) {}
    };

    class RandomActorComponent : public ActorComponent {
    public:
        virtual ~RandomActorComponent() {}
        void update(Actor& actor, World& world) override;
    };
}

#endif
