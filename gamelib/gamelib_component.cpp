#include "pch.h"
#include <gamelib_random.hpp>
#include <gamelib_locator.hpp>
#include <gamelib_component.hpp>
#include "collision.h"
#include <ostream>

namespace GameLib {
    void SimpleInputComponent::update(Actor& actor) {
        auto axis1 = Locator::getInput()->axis1X;
        if (axis1)
            actor.velocity.x = axis1->getAmount();
        auto axis2 = Locator::getInput()->axis1Y;
        if (axis2)
            actor.velocity.y = axis2->getAmount();
    }

    void NoInputComponent::update(Actor& actor) {}

    void RandomInputComponent::update(Actor& actor) {
        actor.velocity.x = random.normal();
        actor.velocity.y = random.normal();
        glm::normalize(actor.velocity);
    }

    void SimpleActorComponent::update(Actor& actor, World& world) {
        // We could make decisions here, start actions, etc
    }

    void RandomActorComponent::update(Actor& actor, World& world) {
        for (auto& a : world.actors) {
            if (a->getId() == actor.getId())
                continue;
        }
    }

	int count = 0;
    void SimplePhysicsComponent::update(Actor& actor, World& world) {
        actor.position += actor.dt * actor.speed * actor.velocity;
        if (actor.clipToWorld) {
            actor.position.x = clamp<float>(actor.position.x, 0, (float)world.worldSizeX - actor.size.x);
            actor.position.y = clamp<float>(actor.position.y, 0, (float)world.worldSizeY - actor.size.y);
        }
    }

    void SimpleGraphicsComponent::update(Actor& actor, Graphics& graphics) {
        glm::vec3 tileSize{ graphics.getTileSizeX(), graphics.getTileSizeY(), 0 };
        glm::vec3 pos = actor.position * tileSize;
        int flipFlags = actor.spriteFlipX ? 1 : actor.spriteFlipY ? 2 : 0;
        graphics.draw(actor.spriteLibId, actor.spriteId, pos.x, pos.y, flipFlags);
    }

    void ColliderComponent::update(Actor& actor, World& world) {
        collider.x = actor.position.x;
        collider.y = actor.position.y;
        collider.w = actor.size.x;
        collider.h = actor.size.y;
        for (auto acts : world.actors) {
            if (actor.spriteId != acts->spriteId) {
                Collision::collisionDetector(actor, *acts);
            }
        }
    }
}
