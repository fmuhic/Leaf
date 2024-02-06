#pragma once

#include <vector>

#include "geometry.h"
#include "physics.h"
#include "types.h"
#include "input.h"

struct Game {
    Game(i32 maxEntityCount);
    ~Game();

    void update(f32 dt, MouseInput &mInput);
    void render();
    void createStackingScene();
    void createDefaultScene();

    std::vector<Entity> entities;
    Geometry* geometry;

    private:

    void reset();
    Entity* findFreeEntity();
    void processInput(MouseInput &mInput);

    void createImmovableGround();

    Physics* physics;
};
