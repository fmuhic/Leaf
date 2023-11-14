#pragma once

#include "state.h"

struct CollisionResult {
    bool colided = false;
    f32 depth;
    glm::vec3 direction;
};

struct Projection {
    f32 min;
    f32 max;
};

void checkCollisions(Game *game);
