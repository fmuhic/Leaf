#pragma once

#include <stack>
#include <vector>

#include "geometry.h"
#include "leaf.h"
#include "physics.h"
#include "types.h"
#include "input.h"
#include "entity_system.h"
#include "examples/example.h"

struct Game {
    Game(i32 maxEntityCount, Leaf* leaf, EntitySystem* entitySytem);
    ~Game();

    void update(f32 dt, f32 elapsed, MouseInput &mInput);
    void changeScene(Example *example);

    std::vector<Entity> entities;
    Geometry* geometry;

    private:

    void updateLogic(f32 elapsed);
    void reset();
    Entity* findFreeEntity();
    void processInput(MouseInput &mInput);

    Physics* physics = nullptr;
    Example* example = nullptr;
    Leaf* leaf = nullptr;
    EntitySystem* entitySystem = nullptr;
    std::stack<i32> ids;
};
