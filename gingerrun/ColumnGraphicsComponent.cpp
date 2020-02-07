#include "ColumnGraphicsComponent.hpp"

void ColumnGraphicsComponent::draw(GameLib::Actor& actor, GameLib::Graphics& graphics) {
    glm::vec3 tileSize{ graphics.getTileSizeX(), graphics.getTileSizeY(), 0 };
    int flipFlags = actor.spriteFlipX ? 1 : actor.spriteFlipY ? 2 : 0;

    for (int i = (int)actor.position.y; i < 23; i++) {
        glm::vec3 pos = actor.position * tileSize;
        pos.y = i * tileSize.y;
        graphics.draw(actor.spriteLibId, actor.spriteId, (int)pos.x, (int)pos.y, flipFlags);
    }
}
