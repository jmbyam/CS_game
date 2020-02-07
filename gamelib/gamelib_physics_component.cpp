#include "pch.h"
#include "gamelib_physics_component.hpp"

namespace GameLib {

    void SimplePhysicsComponent::update(Actor& a, World& w) {
        a.position += a.dt * a.speed * a.velocity;
        if (a.clipToWorld) {
            a.position.x = clamp<float>(a.position.x, 0, (float)w.worldSizeX - a.size.x);
            a.position.y = clamp<float>(a.position.y, 0, (float)w.worldSizeY - a.size.y);
        }
    }

    bool SimplePhysicsComponent::collideWorld(Actor& a, World& w) {
        int aix = (int)a.position.x;
        int aiy = (int)a.position.y;
        bool fracX = a.position.x - aix > 0;
        bool fracY = a.position.y - aiy > 0;
        if (w.getTile(aix, aiy).flags != 0)
            return true;
        if (fracX && w.getTile(aix + 1, aiy).flags != 0)
            return true;
        if (fracY && w.getTile(aix, aiy + 1).flags != 0)
            return true;
        if (fracX && fracY && w.getTile(aix + 1, aiy + 1).flags != 0)
            return true;
        return false;
    }

    bool SimplePhysicsComponent::collideDynamic(Actor& a, Actor& b) { return collides(a, b); }

    bool SimplePhysicsComponent::collideStatic(Actor& a, Actor& b) { return collides(a, b); }

    bool SimplePhysicsComponent::collideTrigger(Actor& a, Actor& b) { return collides(a, b); }
}
