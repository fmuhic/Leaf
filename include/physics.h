#pragma once

#include <vector>

#include "body.h"
#include "state.h"
#include "geometry.h"

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
void resolveCollisions(Game *game);
f32 calculateMomentOfInertia(Entity *e);

struct Physics {
    void resolveCollisions(std::vector<Collision>& collisions, std::vector<Entity>& entities);
    void applyImpulses(Collision& c, RigidBody& a, RigidBody& b);
};
