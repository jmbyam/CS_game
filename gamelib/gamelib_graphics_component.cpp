#include "pch.h"
#include <gamelib_graphics_component.hpp>

namespace GameLib {
    void SimpleGraphicsComponent::draw(Actor& actor, Graphics& graphics) {
        glm::vec3 tileSize{ graphics.getTileSizeX(), graphics.getTileSizeY(), 0 };
        glm::vec3 pos = actor.position * tileSize;
        int flipFlags = actor.spriteFlipX ? 1 : actor.spriteFlipY ? 2 : 0;
        graphics.draw(actor.spriteLibId, actor.spriteId, (int)pos.x, (int)pos.y, flipFlags);
    }
}
