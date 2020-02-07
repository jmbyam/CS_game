#ifndef DUNGEON_ACTOR_COMPONENT_HPP
#define DUNGEON_ACTOR_COMPONENT_HPP

#include <gamelib_actor_component.hpp>

namespace GameLib {
    class DungeonActorComponent : public ActorComponent {
    public:
        virtual ~DungeonActorComponent();

        void handleCollisionWorld(Actor& a, World& w) override;
    };
}
#endif
