#include "DungeonActorComponent.hpp"

namespace GameLib {
    void DungeonActorComponent::handleCollisionWorld(Actor& a, World& w) {
        // determine whether to move the player up, or to the left
        int ix = (int)(a.position.x);
        int iy = (int)(a.position.y);
        float fx = a.position.x - ix;
        float fy = a.position.y - iy;

        bool left = w.getTile(ix, iy).flags;
        bool right = w.getTile(ix + 1, iy).flags;
        bool up = w.getTile(ix, iy).flags;
        bool down = w.getTile(ix, iy + 1).flags;

        if (right && !left && fx < 0.5)
            a.position.x = ix;
        if (left && !right && fx > 0.5)
            a.position.x = ix + 1;

        if (down && !up && fy < 0.5)
            a.position.y = iy;
        if (up && !down && fy > 0.5)
            a.position.y = iy + 1;
        // if (fy < 0.25)
        //    a.position.y = iy;
        // if (fy > 0.25)
        //    a.position.y = iy + 1;
    }
}
