#pragma once

#include <vector>
#include <assert.h>

#include "geometry.h"
#include "physics.h"
#include "types.h"
#include "input.h"

struct World {
    World(i32 maxEntityCount);
    ~World();

    void update(f32 dt, MouseInput &mInput);
    void render();
    void createStackingScene();

    std::vector<Entity> entities;
    Geometry* geometry;

    private:

    Entity* findFreeEntity();
    void processInput(MouseInput &mInput);

    Physics* physics;
};
