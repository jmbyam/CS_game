#pragma once
#include <gamelib.hpp>

class ColumnActorComponent : public GameLib::ActorComponent {
public:
    virtual ~ColumnActorComponent() {}
    void update(GameLib::Actor& actor, GameLib::World& world);

private:
    int baseHeight{ 0 };
};
