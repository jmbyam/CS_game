#ifndef DYNAMIC_ACTOR_COMPONENT_HPP
#define DYNAMIC_ACTOR_COMPONENT_HPP

#include <gamelib_actor_component.hpp>

namespace GameLib {
    class DynamicActorComponent : public ActorComponent {
    public:
        virtual ~DynamicActorComponent() {}
        virtual void handleCollisionDynamic(Actor& a, Actor& b);
    };

}
#endif
