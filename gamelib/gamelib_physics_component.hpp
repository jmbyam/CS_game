#ifndef GAMELIB_PHYSICS_COMPONENT_HPP
#define GAMELIB_PHYSICS_COMPONENT_HPP

#include <gamelib_actor.hpp>
#include <gamelib_world.hpp>

namespace GameLib {
    class PhysicsComponent {
    public:
        virtual ~PhysicsComponent() {}
        // handles updates of position, velocity, and acceleration
        virtual void update(Actor& actor, World& world) = 0;
        // handles collisions between world and actor
        virtual bool collideWorld(Actor& actor, World& world) { return false; }
        // handles collision between movable actors
        virtual bool collideDynamic(Actor& a, Actor& b) { return false; }
        // handles collision between movable actor and static actor
        virtual bool collideStatic(Actor& a, Actor& b) { return false; }
        // handles collision between movable actor and trigger
        virtual bool collideTrigger(Actor& a, Actor& b) { return false; }
    };

    class SimplePhysicsComponent : public PhysicsComponent {
    public:
        virtual ~SimplePhysicsComponent() {}

        void update(Actor& a, World& w);
        bool collideWorld(Actor& a, World& w) override;
        bool collideDynamic(Actor& a, Actor& b) override;
        bool collideStatic(Actor& a, Actor& b) override;
        bool collideTrigger(Actor& a, Actor& b) override;
    };
}

#endif
