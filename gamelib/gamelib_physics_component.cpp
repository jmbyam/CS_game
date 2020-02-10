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

    bool GameLib::TraceCurtisDynamicActorComponent::collideDynamic(Actor& a, Actor& b) {
        glm::vec3 amin = a.position;
        glm::vec3 amax = a.position + a.size;
        glm::vec3 bmin = b.position;
        glm::vec3 bmax = b.position + b.size;

        bool overlapX = (amin.x <= bmax.x && amax.x >= bmin.x);
        bool overlapY = (amin.y <= bmax.y && amax.y >= bmin.y);
        bool overlapZ = (amin.z <= bmax.z && amax.z >= bmin.z);
        if (a.getId() != b.getId()) {
            if (overlapX && overlapY && overlapZ) {
                if (abs(amin.x - bmin.x) > abs(amin.y - bmin.y)) {
                    if (amin.x >= bmin.x) {
                        // moveX
                        // a.position.x += 1;
                        // right
                        a.position.x = clamp<float>(a.position.x, (float)bmax.x, (float)a.position.x + b.position.x);
                    } else if (amax.x <= bmax.x) {
                        // a.position.x -= 1;
                        // left
                        a.position.x = clamp<float>(a.position.x, 0, (float)b.position.x - b.size.x);
                    }
                } else if (abs(amin.x - bmin.x) < abs(amin.y - bmin.y)) {
                    if (amin.y >= bmin.y) {
                        a.position.y = clamp<float>(a.position.y, (float)bmax.y, (float)a.position.y + b.position.y);
                    } else if (amax.y <= bmax.y) {
                        a.position.y = clamp<float>(a.position.y, 0, (float)b.position.y - b.size.y);
                    }
                }
            }
        }
        return overlapX && overlapY && overlapZ;
    }

    void GameLib::TraceCurtisDynamicActorComponent::update(Actor& a, World& w) {
        a.position += a.dt * a.speed * a.velocity;
        if (a.clipToWorld) {
            a.position.x = clamp<float>(a.position.x, 0, (float)w.worldSizeX - a.size.x);
            a.position.y = clamp<float>(a.position.y, 0, (float)w.worldSizeY - a.size.y);
        }
    }

    bool GameLib::DainNickJosephWorldPhysicsComponent::collideWorld(Actor& a, World& w) {
        
        float subTileSize=1.0;
        for (float x=floor(a.position.x); x<ceil(a.position.x+a.size.x); x+=subTileSize)
        {
            if(w.getTile(x,floor(a.position.y)).flags==1)
            {
                return true;
            }
            if(w.getTile(x,ceil(a.position.y+a.size.y)-subTileSize).flags==1)
            {
                return true;
            }
        }
        for (float y=floor(a.position.y); y<ceil(a.position.y+a.size.y); y+=subTileSize)
        {
            if(w.getTile(floor(a.position.x),y).flags==1)
            {
                return true;
            }
            if(w.getTile(ceil(a.position.x+a.size.x)-subTileSize,y).flags==1)
            {
                return true;
            }
        }
        return false;
    }

    void GameLib::DainNickJosephWorldPhysicsComponent::update(Actor& a, World& w) {
        a.position += a.dt * a.speed * a.velocity;
        if (a.clipToWorld) {
            a.position.x = clamp<float>(a.position.x, 0, (float)w.worldSizeX - a.size.x);
            a.position.y = clamp<float>(a.position.y, 0, (float)w.worldSizeY - a.size.y);
        }
    }
}
