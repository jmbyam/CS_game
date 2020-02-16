#ifndef GAMELIB_ACTOR_HPP
#define GAMELIB_ACTOR_HPP

#include <gamelib_actor_component.hpp>
#include <gamelib_graphics_component.hpp>
#include <gamelib_input_component.hpp>
#include <gamelib_object.hpp>
#include <gamelib_physics_component.hpp>
#include <gamelib_world.hpp>

namespace GameLib {
	class InputComponent;
	class PhysicsComponent;
	class GraphicsComponent;
	class ActorComponent;

	using InputComponentPtr = std::shared_ptr<InputComponent>;
	using ActorComponentPtr = std::shared_ptr<ActorComponent>;
	using PhysicsComponentPtr = std::shared_ptr<PhysicsComponent>;
	using GraphicsComponentPtr = std::shared_ptr<GraphicsComponent>;

	class Actor : public Object {
	public:
		Actor() {}
		Actor(InputComponentPtr input,
			ActorComponentPtr actor,
			PhysicsComponentPtr physics,
			GraphicsComponentPtr graphics);
		virtual ~Actor();

		using weak_ptr = std::weak_ptr<Actor>;
		using shared_ptr = std::shared_ptr<Actor>;
		using const_weak_ptr = const std::weak_ptr<Actor>;
		using const_shared_ptr = const std::shared_ptr<Actor>;

		// returns id of the actor
		unsigned getId() const { return id_; }

		// returns a character description of the actor which is for saving/loading
		virtual char charDesc() const { return charDesc_; }

		InputComponent* inputComponent() { return input_.get(); }
		ActorComponent* actorComponent() { return actor_.get(); }
		PhysicsComponent* physicsComponent() { return physics_.get(); }
		GraphicsComponent* graphicsComponent() { return graphics_.get(); }

		// Called whenever the object is introduced into the game, t is current time
		void beginPlay(float t0);

		// Called each frame the object needs to update itself before drawing
		void update(float deltaTime, World& world);

		// Called each frame for the object to handle collisions and physics
		void physics(float deltaTime, World& world);

		// Called each frame to draw itself (not called for invisible objects)
		void draw(Graphics& graphics);

		// Switches current animation, < 0 restarts current animation
		void switchAnim(int i);

		// returns position as a 2D point
		glm::vec2 position2d() const { return { position.x, position.y }; }

		// returns size as a 2D vector
		glm::vec2 size2d() const { return { size.x, size.y }; }

		// Get world pixel position of the actor's upper left coordinates
		glm::ivec2 pixelPosition(Graphics& g) { return static_cast<glm::ivec2>(g.tileSizef() * position2d()); }

		// returns pixel minimum coordinates (xy * tileSize)
		glm::ivec2 pixelmin(Graphics& g) { return static_cast<glm::ivec2>(g.tileSizef() * position2d()); }

		// returns pixel maximum coordinates (xy * tilesSize - 1)
		glm::ivec2 pixelmax(Graphics& g) {
			return static_cast<glm::ivec2>(g.tileSizef() * (position2d() + size2d())) - glm::ivec2(1);
		}

		// Get pixel size of the actor
		glm::ivec2 pixelSize(Graphics& g) { return static_cast<glm::ivec2>(g.tileSizef() * size2d()); }

		// Return the center position of the actor's
		glm::ivec2 pixelCenter(Graphics& g) {
			glm::ivec4 rect{ (int)(g.getTileSizeX() * position.x),
				(int)(g.getTileSizeY() * position.y),
				(int)(g.getTileSizeX() * size.x),
				(int)(g.getTileSizeY() * size.y) };
			return { rect.x + (rect.z >> 1), rect.y + (rect.w >> 1) };
		}

		using uint = unsigned;
		using ushort = unsigned short;
		using ubyte = unsigned char;
		using ubool = unsigned short; // using short can avoid character integer issues

		////////////////////////////////////////////////////
		// IMAGES AND SPRITES //////////////////////////////
		////////////////////////////////////////////////////

		// image name for this object (for non tile contexts)
		std::string imageName;

		struct SPRITEINFO {
			// sprite number for this object
			uint id{ 0 };
			// sprite library number for this object
			uint libId{ 0 };
			bool flipX{ false };
			bool flipY{ false };

			int flipFlags() const {
				if (flipX)
					return 1;
				if (flipY)
					return 2;
				return 0;
			}
		} sprite;

		// get main sprite id
		uint spriteId() const { return sprite.id; }
		// get main sprite lib id
		uint spriteLibId() const { return sprite.libId; }

		// set main sprite id, configures anim sprite id if 0
		uint setSprite(uint libId, uint id) {
			if (anim.baseId == 0)
				anim.baseId = id;
			sprite.libId = libId;
			sprite.id = id;
			return id;
		}

		////////////////////////////////////////////////////
		// ANIMATION ///////////////////////////////////////
		////////////////////////////////////////////////////

		struct ANIMINFO {
			int baseId{ 0 };
			int count{ 1 };
			float speed{ 8.0f };
			enum { CYCLE, SINGLE, BOUNCE };
			int type{ CYCLE };

			// copies public members of ANIMINFO
			ANIMINFO& operator=(const ANIMINFO& other) {
				baseId = other.baseId;
				count = other.count;
				speed = other.speed;
				type = other.type;
				return *this;
			}

			// returns speed * (t1-t0)
			int framesSinceStart() const { return static_cast<int>(speed * (t1 - t0)); }

			// returns single shot frame within range [baseId, baseId + count)
			int singleFrame() const { return std::min(baseId + count, baseId + framesSinceStart()); }

			// returns cycling frame within range [basedId, baseId + count)
			int cycleFrame() const {
				if (!count)
					return 0;
				return baseId + framesSinceStart() % count;
			}

			// returns bounced frame within range [basedId, baseId + count)
			int bounceFrame() const {
				int c = count - 1;
				int fss = framesSinceStart() % (c << 1);
				return baseId + c - std::abs(fss - c);
			}

			int currentFrame() const {
				switch (type) {
				case CYCLE: return cycleFrame();
				case SINGLE: return singleFrame();
				case BOUNCE: return bounceFrame();
				}
				return cycleFrame();
			}

			// sets up quick animation data
			void reset(int _baseId, int _count, float _speed, int _type = CYCLE) {
				baseId = _baseId;
				count = _count;
				speed = _speed;
			}

			// sets animation to new start time
			void start(float t) {
				t0 = t;
				t1 = t;
			}

			// returns whether animation has reached last frame
			bool ended() const { return int(speed * (t1 - t0)) > count; }

			// updates current time, returns current frame
			int update(float dt) {
				t1 += dt;
				return currentFrame();
			}

		private:
			float t0{ 0 };
			float t1{ 0 };
		} anim;

		// use this to store animation sequences
		std::vector<ANIMINFO> anims;

		////////////////////////////////////////////////////
		// FLAGS ///////////////////////////////////////////
		////////////////////////////////////////////////////

		// is object visible for drawing
		ubool visible{ true };
		// is actor active for updating
		ubool active{ true };
		// is object used for physics
		ubool clipToWorld{ true };
		// is object unable to move
		ubool movable{ true };

		////////////////////////////////////////////////////
		// 3D GRAPHICS SUPPORT (EXPERIMENTAL) //////////////
		////////////////////////////////////////////////////

		struct _3DINFO {
			// transform that takes this object to world space
			glm::mat4 transform;
			// additional transform that moves this object in local space
			glm::mat4 addlTransform;
			// minimum coordinates for this bounding box in local space
			glm::vec3 bboxMin;
			// maximum coordinates for this bounding box in world space
			glm::vec3 bboxMax;

			// Gets the world matrix for this actor which is transform * addlTransform
			glm::mat4 worldMatrix() const { return transform * addlTransform; }

			// Gets column 4 of the world matrix which is the local origin in world space
			glm::vec3 worldPosition() const {
				glm::mat4 w = worldMatrix();
				return glm::vec3{ w[3][0], w[3][1], w[3][2] };
			}

			// Gets the minimum bounds for this actor in world space, bbox is not rotated
			glm::vec3 worldBBoxMin() const { return worldPosition() + bboxMin; }

			// Gets the maximum bounds for this actor in world space, bbox is not rotated
			glm::vec3 worldBBoxMax() const { return worldPosition() + bboxMax; }
		} _3d;

		////////////////////////////////////////////////////
		// TIMING INFORMATION //////////////////////////////
		////////////////////////////////////////////////////

		// time elapsed for next update
		float dt;
		float t0; // time since start
		float t1; // current time

		////////////////////////////////////////////////////
		// POSITION AND VELOCITY ///////////////////////////
		////////////////////////////////////////////////////

		// current position (in world units)
		glm::vec3 position{ 0.0f, 0.0f, 0.0f };
		glm::vec3 lastPosition{ 0.0f, 0.0f, 0.0f };
		glm::vec3 dPosition{ 0.0f, 0.0f, 0.0f };

		// size (in world units, assume 1 = grid size)
		glm::vec3 size{ 1.0f, 1.0f, 1.0f };

		glm::vec3 min() const { return position; }
		glm::vec3 max() const { return position + size; }
		glm::vec3 center() const { return position + size * 0.5f; }

		// current velocity (in world units)
		glm::vec3 velocity{ 0.0f, 0.0f, 0.0f };

		// maximum speed (in world units)
		float speed{ 1.0f };

		struct PHYSICSINFO {
			float mass{ 1.0f };
			glm::vec3 v{ 0.0f, 0.0f, 0.0f };
			glm::vec3 v_t{ 0.0f, 0.0f, 0.0f };
			glm::vec3 v_n{ 0.0f, 0.0f, 0.0f };
		} physicsInfo;

		////////////////////////////////////////////////////
		// DYNAMIC, STATIC, TRIGGER INFORMATION ////////////
		////////////////////////////////////////////////////

		struct TRIGGERINFO {
			bool overlapping{ false };
			ActorWPtr triggerActor;
		} triggerInfo;

		enum { NONE = 0, DYNAMIC = 1, STATIC = 2, TRIGGER = 4 };

		bool isDynamic() const { return type_ == DYNAMIC; }
		bool isStatic() const { return type_ == STATIC; }
		bool isTrigger() const { return type_ == TRIGGER; }

		void makeDynamic();
		void makeStatic();
		void makeTrigger();

	protected:
		std::string _updateDesc() override { return { "Actor" }; }
		std::string _updateInfo() override { return { "Actor" }; }
		char charDesc_ = '?';
		unsigned id_{ 0 };
		int type_{ NONE };

	private:
		InputComponentPtr input_;
		ActorComponentPtr actor_;
		PhysicsComponentPtr physics_;
		GraphicsComponentPtr graphics_;

		static unsigned idSource_;
	}; // namespace GameLib

	using ActorPtr = std::shared_ptr<Actor>;

	template <class... U>
	ActorPtr makeActor(const std::string& name, U&&... _Args) {
		ActorPtr a = std::make_shared<Actor>(std::forward<U>(_Args)...);
		a->rename(name);
		return a;
	}

	inline bool collides(GameLib::Actor& a, GameLib::Actor& b) {
		glm::vec3 amin = a.position;
		glm::vec3 amax = a.position + a.size;
		glm::vec3 bmin = b.position;
		glm::vec3 bmax = b.position + b.size;

		bool overlapX = (amin.x <= bmax.x && amax.x >= bmin.x);
		bool overlapY = (amin.y <= bmax.y && amax.y >= bmin.y);
		bool overlapZ = (amin.z <= bmax.z && amax.z >= bmin.z);
		return overlapX && overlapY && overlapZ;
	}

	inline bool pointInside(glm::vec3 p, GameLib::Actor& a) {
		glm::vec3 amin = a.position;
		glm::vec3 amax = a.position + a.size;
		bool insideX = amin.x >= p.x && amax.x <= p.x;
		bool insideY = amin.y >= p.y && amax.y <= p.y;
		bool insideZ = amin.z >= p.z && amax.z <= p.z;
		return insideX && insideY && insideZ;
	}
} // namespace GameLib

#endif
