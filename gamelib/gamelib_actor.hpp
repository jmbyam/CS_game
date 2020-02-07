#ifndef GAMELIB_ACTOR_HPP
#define GAMELIB_ACTOR_HPP

#include <gamelib_object.hpp>
#include <gamelib_world.hpp>
#include <gamelib_input_component.hpp>
#include <gamelib_actor_component.hpp>
#include <gamelib_physics_component.hpp>
#include <gamelib_graphics_component.hpp>

namespace GameLib {
    class InputComponent;
    class PhysicsComponent;
    class GraphicsComponent;
    class ActorComponent;

    class Actor : public Object {
    public:
        Actor(InputComponent* input, ActorComponent* actor, PhysicsComponent* physics, GraphicsComponent* graphics);
        virtual ~Actor();

        using weak_ptr = std::weak_ptr<Actor>;
        using shared_ptr = std::shared_ptr<Actor>;
        using const_weak_ptr = const std::weak_ptr<Actor>;
        using const_shared_ptr = const std::shared_ptr<Actor>;

        // returns id of the actor
        unsigned getId() const { return id_; }
        // returns a character description of the actor which is for saving/loading
        virtual char charDesc() const { return charDesc_; }

        InputComponent* inputComponent() { return input_; }
        ActorComponent* actorComponent() { return actor_; }
        PhysicsComponent* physicsComponent() { return physics_; }
        GraphicsComponent* graphicsComponent() { return graphics_; }

        // Called whenever the object is introduced into the game
        void beginPlay();

        // Called each frame the object needs to update itself before drawing
        void update(float deltaTime, World& world);

        // Called each frame for the object to handle collisions and physics
        void physics(float deltaTime, World& world);

        // Called each frame to draw itself (not called for invisible objects)
        void draw(Graphics& graphics);

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

        using uint = unsigned;
        using ushort = unsigned short;
        using ubyte = unsigned char;
        using ubool = unsigned short; // using short can avoid character integer issues

        // sprite number for this object
        uint spriteId{ 0 };
        // sprite library number for this object
        uint spriteLibId{ 0 };
        bool spriteFlipX{ false };
        bool spriteFlipY{ false };

        // is object visible for drawing
        ubool visible{ true };
        // is actor active for updating
        ubool active{ true };
        // is object used for physics
        ubool clipToWorld{ true };
        // is object unable to move
        ubool movable{ true };

        // transform that takes this object to world space
        glm::mat4 transform;
        // additional transform that moves this object in local space
        glm::mat4 addlTransform;
        // minimum coordinates for this bounding box in local space
        glm::vec3 bboxMin;
        // maximum coordinates for this bounding box in world space
        glm::vec3 bboxMax;

        // time elapsed for next update
        float dt;

        // current position
        glm::vec3 position{ 0.0f, 0.0f, 0.0f };

        // size (assume 1 = grid size)
        glm::vec3 size{ 1.0f, 1.0f, 1.0f };

        // current velocity
        glm::vec3 velocity{ 0.0f, 0.0f, 0.0f };

        // maximum speed
        float speed{ 1.0f };

        struct TRIGGERINFO {
            bool overlapping{ false };
        } triggerInfo;

    protected:
        std::string _updateDesc() override { return { "Actor" }; }
        std::string _updateInfo() override { return { "Actor" }; }
        char charDesc_ = '?';
        unsigned id_{ 0 };

    private:
        InputComponent* input_{ nullptr };
        PhysicsComponent* physics_{ nullptr };
        GraphicsComponent* graphics_{ nullptr };
        ActorComponent* actor_{ nullptr };

        static unsigned idSource_;
    };

    inline bool collides(GameLib::Actor& a, GameLib::Actor& b) {
        glm::vec3 amin = a.position;
        glm::vec3 amax = a.position + a.size;
        glm::vec3 bmin = b.position;
        glm::vec3 bmax = b.position + b.size;

        bool overlapX = (amin.x <= bmax.x && amax.x >= bmin.x);
        bool overlapY = (amin.y <= bmax.y && amax.y >= bmin.z);
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
}

#endif
