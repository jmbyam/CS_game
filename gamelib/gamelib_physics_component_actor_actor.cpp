#include "gamelib_physics_component_actor_actor.hpp"

bool GameLib::TraceCurtisDynamicActorComponent::collideDynamic(Actor& a, Actor& b) { 
	glm::vec3 amin = a.position;
    glm::vec3 amax = a.position + a.size;
    glm::vec3 bmin = b.position;
    glm::vec3 bmax = b.position + b.size; 
	
	bool overlapX = (amin.x <= bmax.x && amax.x >= bmin.x);
    bool overlapY = (amin.y <= bmax.y && amax.y >= bmin.z);
    bool overlapZ = (amin.z <= bmax.z && amax.z >= bmin.z);

	if (overlapX) {
        if (amin.x >= bmin.x) {
            a.position.x -= 1;
        } else if (amax.x <= bmax.x) {
            a.position.x += 1;
		}
        return true;
	}

	return false;
}

void GameLib::TraceCurtisDynamicActorComponent::update(Actor& a, World& w) {
    a.position += a.dt * a.speed * a.velocity;
    if (a.clipToWorld) {
        a.position.x = clamp<float>(a.position.x, 0, (float)w.worldSizeX - a.size.x);
        a.position.y = clamp<float>(a.position.y, 0, (float)w.worldSizeY - a.size.y);
    }
}
