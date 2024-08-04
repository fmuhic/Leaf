#pragma once

#include <vector>

#include "broad_phase.h"
#include "geometry.h"
#include "physics.h"
#include "types.h"
#include "input.h"
#include "dynamic_tree.h"
#include "examples/example.h"

struct Game {
    Game(i32 maxEntityCount);
    ~Game();

    void update(f32 dt, f32 elapsed, MouseInput &mInput);
    void changeScene(Example *example);
    void debugTree(std::vector<std::pair<AABB, i32>>& boxes);

    std::map<CollisionKey, Collision> candidatesPool = std::map<CollisionKey, Collision>();
    std::vector<Entity> entities;
    Geometry* geometry;

    private:

    void updateLogic(f32 elapsed);
    void reset();
    void processInput(MouseInput &mInput);
    void updateCandidates();

    Physics* physics = nullptr;
    Example* example = nullptr;
    BroadPhase* broadPhase = nullptr;
    DynamicTree* tree = nullptr;

    std::vector<i32> moves;
    std::vector<i32> tempStack;
};
