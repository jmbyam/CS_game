#include "pch.h"
#include <gamelib_locator.hpp>
#include <gamelib_graphics_component.hpp>

namespace GameLib {
    void debugDraw(Actor& a) {
        World* world = Locator::getWorld();
        auto graphics = Locator::getGraphics();

        if (world && graphics) {
            int ix1 = (int)(a.position.x);
            int iy1 = (int)(a.position.y);
            int ix2 = ix1 + 1;
            int iy2 = iy1 + 1;
            int tl = world->getTile(ix1, iy1).flags ? 9 : 8;
            int tr = world->getTile(ix2, iy1).flags ? 9 : 8;
            int bl = world->getTile(ix1, iy2).flags ? 9 : 8;
            int br = world->getTile(ix2, iy2).flags ? 9 : 8;
            float fx = fract(a.position.x);
            float fy = fract(a.position.y);
            int w = graphics->getTileSizeX();
            int h = graphics->getTileSizeY();
            ix1 *= w;
            ix2 *= w;
            iy1 *= h;
            iy2 *= h;
            graphics->draw(LIBXOR_TILESET32, tl, ix1, iy1);
            graphics->draw(LIBXOR_TILESET32, tr, ix2, iy1);
            graphics->draw(LIBXOR_TILESET32, bl, ix1, iy2);
            graphics->draw(LIBXOR_TILESET32, br, ix2, iy2);
            int dx = ix1 + (w << 1);
            int dy = iy1;
            int dw = 8;
            int dh = 8;
            if (fx < 0.5f)
                graphics->draw(dx, dy, dw, dh, Green);
            if (fx > 0.5f)
                graphics->draw(dx, dy, dw, dh, Red);
            dx += dw;
            if (fy < 0.5f)
                graphics->draw(dx, dy, dw, dh, Green);
            if (fy > 0.5f)
                graphics->draw(dx, dy, dw, dh, Red);
            dx += dw;
            if (fx < fy)
                graphics->draw(dx, dy, dw, dh, Violet);
            else if (fx > fy)
                graphics->draw(dx, dy, dw, dh, Gold);
            dx += dw;
            if (fx > 0.99f)
                graphics->draw(dx, dy, dw, dh, Green);
            else
                graphics->draw(dx, dy, dw, dh, Red);
            dx += dw;
            if (fy < 0.99f)
                graphics->draw(dx, dy, dw, dh, Green);
            else
                graphics->draw(dx, dy, dw, dh, Red);
        }
    }

    void SimpleGraphicsComponent::draw(Actor& actor, Graphics& graphics) {
        glm::vec3 tileSize{ graphics.getTileSizeX(), graphics.getTileSizeY(), 0 };
        glm::vec3 pos = actor.position * tileSize;
        int flipFlags = actor.spriteFlipX ? 1 : actor.spriteFlipY ? 2 : 0;
        graphics.draw(actor.spriteLibId, actor.spriteId, (int)pos.x, (int)pos.y, flipFlags);
    }

    void DebugGraphicsComponent::draw(Actor& actor, Graphics& graphics) {
        glm::vec3 tileSize{ graphics.getTileSizeX(), graphics.getTileSizeY(), 0 };
        glm::vec3 pos = actor.position * tileSize;
        glm::vec3 size = actor.size * tileSize;
        int flipFlags = actor.spriteFlipX ? 1 : actor.spriteFlipY ? 2 : 0;
        graphics.draw(actor.spriteLibId, actor.spriteId, (int)pos.x, (int)pos.y, flipFlags);

		debugDraw(actor);
        graphics.draw((int)pos.x, (int)pos.y, (int)size.x, (int)size.y, ForestGreen);
    }
}
