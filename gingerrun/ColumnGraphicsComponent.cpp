#include "ColumnGraphicsComponent.hpp"

void ColumnGraphicsComponent::draw(GameLib::Actor& actor, GameLib::Graphics& graphics) {
    glm::vec3 tileSize{ graphics.getTileSizeX(), graphics.getTileSizeY(), 0 };

    for (int i = (int)actor.position.y; i < 23; i++) {
        glm::vec3 pos = actor.position * tileSize;
        pos.y = i * tileSize.y;
        graphics.draw(actor.sprite.libId, actor.sprite.id, (int)pos.x, (int)pos.y, actor.sprite.flipFlags());
    }
}
