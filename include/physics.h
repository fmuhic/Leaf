#pragma once

#include "state.h"

struct CollisionResult {
    bool colided = false;
    f32 depth;
    glm::vec3 direction;
};

void checkCollisions(Game *game);
