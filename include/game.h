#pragma once

#include <vector>

#include "geometry.h"
#include "physics.h"
#include "types.h"
#include "input.h"
#include "examples/example.h"

struct Game {
    Game(i32 maxEntityCount);
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
};
