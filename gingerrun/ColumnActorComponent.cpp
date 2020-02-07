#include "ColumnActorComponent.hpp"

void ColumnActorComponent::update(GameLib::Actor& actor, GameLib::World& world) {
    if (actor.position.x + 1 < 0) {
        actor.position.x = (float)world.worldSizeX;

        int units = (int)(world.worldSizeY >> 2);
        baseHeight = (units << 1) + GameLib::random.rd() % units + units;
    }
    actor.position.y = (float)baseHeight;
}
