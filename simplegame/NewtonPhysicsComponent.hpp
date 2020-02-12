#ifndef NEWTON_PHYSICS_COMPONENT_HPP
#define NEWTON_PHYSICS_COMPONENT_HPP

#include <gamelib_actor.hpp>

namespace GameLib {
	class NewtonPhysicsComponent: public SimplePhysicsComponent {
	public:
		void update(Actor& a, World& world) override;
	};
} // namespace GameLib

#endif
