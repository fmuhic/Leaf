#pragma once

#include "entity_system.h"
#include "leaf.h"
#include "types.h"
#include "input.h"
#include "examples/example.h"

struct Game {
    Game(Leaf* leaf, EntitySystem* entitySystem);
    ~Game();

    void update(f32 dt, f32 elapsed, Input &input);
    void changeScene(Example *example);
    void debugTree(std::vector<std::pair<AABB, i32>>& boxes);

    private:

    void updateLogic(f32 elapsed);
    void reset();
    void processInput(Input &input);

    Leaf* leaf;
    EntitySystem* entitySystem;
    Example* example = nullptr;
};
