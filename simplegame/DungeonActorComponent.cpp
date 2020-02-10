#include "DungeonActorComponent.hpp"

namespace GameLib {
    void DungeonActorComponent::handleCollisionWorld(Actor& a, World& w) {
        // determine whether to move the player up, or to the left
        int ix = (int)(a.position.x);
        int iy = (int)(a.position.y);
        float fx = a.position.x - ix;
        float fy = a.position.y - iy;
        float gx = 1 - fx;
        float gy = 1 - fy;

        bool tl = w.getTile(ix, iy).flags;
        bool tr = w.getTile(ix + 1, iy).flags;
        bool bl = w.getTile(ix, iy + 1).flags;
        bool br = w.getTile(ix + 1, iy + 1).flags;
        int collision = 0;
        collision |= tl ? 1 : 0;
        collision |= tr ? 2 : 0;
        collision |= bl ? 4 : 0;
        collision |= br ? 8 : 0;
        float x1 = ix;
        float x2 = ix + 1.0f;
        float y1 = iy;
        float y2 = iy + 1.0f;
        switch (collision) {
        case 0: // no collision
            break;
        case 1: // top left single block
            if (fy < fx)
                a.position.x = x2;
            else
                a.position.y = y2;
            break;
        case 2: // top right single block
            if (fy < 0.99f)
                a.position.x = x1;
            else
                a.position.y = y2;
            break;
        case 3: // top wall
            a.position.y = y2;
            break;
        case 4: // bottom left single block
            if (fx > 0.99f)
                a.position.x = x2;
            else
                a.position.y = y1;
            break;
        case 5: // left wall
            a.position.x = x2;
            break;
        case 6: // 
            if (fy < 0.5f)
                a.position.x = x1;
            if (fy < 0.5f && fy < 1.0f)
                a.position.y = y1;
            if (fy > 0.5f)
                a.position.x = x2;
            if (fy > 0.5f && fx > 0.0f)
                a.position.y = y2;
            break;
        case 7: // top left wall
            a.position.x = x2;
            a.position.y = y2;
            break;
        case 8: // bottom right single block
            if (fy > fx)
                a.position.x = x1;
            else
                a.position.y = y1;
            break;
        case 9:
            if (fx < 0.5f)
                a.position.x = x1;
            if (fx < 0.5f && fy < 1.0f)
                a.position.y = y2;
            if (fx > 0.5f)
                a.position.x = x2;
            if (fx > 0.5f && fy > 0.0f)
                a.position.y = y1;
            break;
        case 10: // right wall
            a.position.x = x1;
            break;
        case 11: // top right wall
            a.position.x = x1;
            a.position.y = y2;
            break;
        case 12: // bottom wall
            a.position.y = y1;
            break;
        case 13: // bottom left wall
            a.position.x = x2;
            a.position.y = y1;
            break;
        case 14: // bottom right wall
            a.position.x = x1;
            a.position.y = y1;
            break;
        case 15:
            break;
        }
        // if (tr && !tl && fx < 0.5)
        //    a.position.x = ix;
        // if (tl && !tr && fx > 0.5)
        //    a.position.x = ix + 1;

        // if (down && !bl && fy < 0.5)
        //    a.position.y = iy;
        // if (bl && !down && fy > 0.5)
        //    a.position.y = iy + 1;
        // if (fy < 0.25)
        //    a.position.y = iy;
        // if (fy > 0.25)
        //    a.position.y = iy + 1;
    }
}
