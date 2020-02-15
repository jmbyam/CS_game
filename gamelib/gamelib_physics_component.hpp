#ifndef GAMELIB_PHYSICS_COMPONENT_HPP
#define GAMELIB_PHYSICS_COMPONENT_HPP

#include <gamelib_actor.hpp>
#include <gamelib_world.hpp>

namespace GameLib {
	class PhysicsComponent {
	public:
		virtual ~PhysicsComponent() {}
		// handles updates of position, velocity, and acceleration
		virtual void update(Actor& actor, World& world) {}
		// handles collisions between world and actor
		virtual bool collideWorld(Actor& actor, World& world) { return false; }
		// handles collision between movable actors
		virtual bool collideDynamic(Actor& a, Actor& b) { return false; }
		// handles collision between movable actor and static actor
		virtual bool collideStatic(Actor& a, Actor& b) { return false; }
		// handles collision between movable actor and trigger
		virtual bool collideTrigger(Actor& a, Actor& b) { return false; }
	};

	class SimplePhysicsComponent : public PhysicsComponent {
	public:
		virtual ~SimplePhysicsComponent() {}

		void update(Actor& a, World& w) override;
		bool collideWorld(Actor& a, World& w) override;
		bool collideDynamic(Actor& a, Actor& b) override;
		bool collideStatic(Actor& a, Actor& b) override;
		bool collideTrigger(Actor& a, Actor& b) override;
	};

	class TraceCurtisDynamicActorComponent : public PhysicsComponent {
	public:
		~TraceCurtisDynamicActorComponent(){};

		bool collideDynamic(Actor& a, Actor& b) override;

		void update(Actor& a, World& w) override;
	};

	class DainNickJosephWorldPhysicsComponent : public PhysicsComponent {
	public:
		~DainNickJosephWorldPhysicsComponent(){};

		bool collideWorld(Actor& actor, World& world) override;

		void update(Actor& a, World& w) override;
	};

	class TailonsDynamicPhysicsComponent : public PhysicsComponent {
	public:
		~TailonsDynamicPhysicsComponent(){};

		bool collideDynamic(Actor& a, Actor& b) override;

		void update(Actor& a, World& w) override;
	};

	class TailonsStaticPhysicsComponent : public PhysicsComponent {
	public:
		~TailonsStaticPhysicsComponent(){};

		bool collideStatic(Actor& a, Actor& b) override;

		void update(Actor& a, World& w) override;
	};
} // namespace GameLib

#endif
