#ifndef GAMELIB_PHYSICS_TRACE_CURTIS_DYNAMIC_ACTOR_COMPONENT_HPP
#define GAMELIB_PHYSICS_TRACE_CURTIS_DYNAMIC_ACTOR_COMPONENT_HPP

#include <gamelib_actor.hpp>
#include <gamelib_physics_component.hpp>

namespace GameLib {

    class TraceCurtisDynamicActorComponent : public PhysicsComponent {
        ~TraceCurtisDynamicActorComponent(){};

        bool collideDynamic(Actor& a, Actor& b) override;

        void update(Actor& a, World& w) override;
    };
}

#endif
