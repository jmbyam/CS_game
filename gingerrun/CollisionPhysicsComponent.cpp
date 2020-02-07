#include "CollisionPhysicsComponent.hpp"

void CollisionPhysicsComponent::update(GameLib::Actor& actor, GameLib::World& world) {
    glm::vec3 prevPosition = actor.position;

    constexpr float vx = 1;
    constexpr float h = 8;
    constexpr float x = 4;
    constexpr float v0 = 2 * h * vx / x;

    constexpr float gpos = 2 * h * vx * vx / (x * x);
    constexpr float gneg = 6 * h * vx * vx / (x * x);

    float g = gpos;
    if (actor.velocity.y < 0)
        g = gneg;
    actor.velocity.y += actor.dt * g;

    actor.position += actor.dt * actor.velocity;
    if (actor.clipToWorld) {
		using GameLib::clamp;
        actor.position.x = clamp<float>(actor.position.x, 0, (float)world.worldSizeX - actor.size.x);
        actor.position.y = clamp<float>(actor.position.y, 0, (float)world.worldSizeY - actor.size.y);
    }
    if (prevPosition.y == actor.position.y) {
        actor.velocity.y = 0;
    }

    GameLib::Actor* actorA = &actor;
    // do collision detection
    for (auto& actorB : world.dynamicActors) {
        if (!actorB->active)
            continue;
        if (actorB->getId() == actor.getId())
            continue;
        if (collides(*actorB, actor)) {
            // back both up a tiny slice at a time
            const float minidt = 0.001f;
            for (int i = 0; i < 100; i++) {
                if (actorB->movable)
                    actorB->position -= actorB->velocity * minidt;
                if (actorA->movable)
                    actorA->position -= actorA->velocity * minidt;
                if (!collides(*actorA, *actorB))
                    break;
            }
            // if actors are still colliding, move A up or to the side
            if ((actorA->movable && !actorB->movable) && collides(*actorA, *actorB)) {
                float horz = actorA->position.x - actorB->position.x;
                float vert = actorA->position.y - actorB->position.y;
                bool moveup = std::abs(horz) > std::abs(vert);
                if (moveup) {
                    if (vert < 0) {
                        actorA->position.y = actorB->position.y - actorA->size.y;
                    } else {
                        actorA->position.y = actorB->position.y + actorB->size.y;
                    }
                } else {
                    if (horz < 0) {
                        actorA->position.x = actorB->position.x - actorA->size.x;
                    } else {
                        actorA->position.x = actorB->position.x + actorB->size.x;
                    }
                }
                if (collides(*actorA, *actorB)) {
                    actorA->position.y = 0;
                }
            }

            // HFLOGDEBUG("boom! between %d and %d", actorB->getId(), actor.getId());
        }
    }
}

bool CollisionPhysicsComponent::collides(GameLib::Actor& a, GameLib::Actor& b) {
    glm::vec3 amin = a.position;
    glm::vec3 amax = a.position + a.size;
    glm::vec3 bmin = b.position;
    glm::vec3 bmax = b.position + b.size;

    bool overlapX = (amin.x <= bmax.x && amax.x >= bmin.x);
    bool overlapY = (amin.y <= bmax.y && amax.y >= bmin.z);
    bool overlapZ = (amin.z <= bmax.z && amax.z >= bmin.z);
    return overlapX && overlapY && overlapZ;
}

bool CollisionPhysicsComponent::pointInside(glm::vec3 p, GameLib::Actor& a) {
    glm::vec3 amin = a.position;
    glm::vec3 amax = a.position + a.size;
    bool insideX = amin.x >= p.x && amax.x <= p.x;
    bool insideY = amin.y >= p.y && amax.y <= p.y;
    bool insideZ = amin.z >= p.z && amax.z <= p.z;
    return insideX && insideY && insideZ;
}
