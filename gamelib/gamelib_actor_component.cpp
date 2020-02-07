#include "pch.h"
#include <gamelib_actor_component.hpp>

namespace GameLib {
    void RandomActorComponent::update(Actor& actor, World& world) {
        for (auto& a : world.dynamicActors) {
            if (a->getId() == actor.getId())
                continue;
        }
    }
}
