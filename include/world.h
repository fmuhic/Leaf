#pragma once

#include <vector>
#include <assert.h>

#include "geometry.h"
#include "types.h"
#include "state.h"

struct World {
    World(i32 maxEntityCount);
    ~World();

    void update(f32 dt, KeyboardInput *kInput, MouseInput *mInput);
    void render();
    void createStackingScene();

    std::vector<Entity> entities;

    private:

    Entity* findFreeEntity();
    void processInput(MouseInput *mInput);

    Geometry* geometry;
};
