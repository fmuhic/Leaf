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

struct PointLineResult {
    glm::vec3 cp;
    f32 distSq;
};

void checkCollisions(Game *game);
void resolveCollisions(Game *game);
f32 calculateMomentOfInertia(Entity *e);
