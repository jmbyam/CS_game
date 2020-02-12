#include "pch.h"
#include <gamelib_actor_component.hpp>

namespace GameLib {
	void RandomActorComponent::update(Actor& actor, World& world) {
		for (auto& a : world.dynamicActors) {
			if (a->getId() == actor.getId())
				continue;
		}
	}

	void DainNickJosephWorldCollidingActorComponent::handleCollisionWorld(Actor& actor, World& world) {
		float subTileSize = 1.0;
		float y = 0;
		int horizontalScore = 0;
		for (float x = floor(actor.position.x); x < ceil(actor.position.x + actor.size.x); x += subTileSize) {
			if (world.getTile(x, floor(actor.position.y)).flags == 1) {
				if (world.getTile(x, floor(actor.position.y) + subTileSize).flags == 0)
					actor.position.y -= actor.dt * actor.speed * actor.velocity.y;
			}
			if (world.getTile(x, ceil(actor.position.y + actor.size.y) - subTileSize).flags == 1) {
				if (world.getTile(x, ceil(actor.position.y + actor.size.y) - 2 * subTileSize).flags == 0)
					actor.position.y -= actor.dt * actor.speed * actor.velocity.y;
			}
		}
		for (float y = floor(actor.position.y); y < ceil(actor.position.y + actor.size.y); y += subTileSize) {
			if (world.getTile(floor(actor.position.x), y).flags == 1) {
				if (world.getTile(floor(actor.position.x) + subTileSize, y).flags == 0)
					actor.position.x -= actor.dt * actor.speed * actor.velocity.x;
			}
			if (world.getTile(ceil(actor.position.x + actor.size.x) - subTileSize, y).flags == 1) {
				if (world.getTile(ceil(actor.position.x + actor.size.x) - 2 * subTileSize, y).flags == 0)
					actor.position.x -= actor.dt * actor.speed * actor.velocity.x;
			}
		}
	}
} // namespace GameLib
