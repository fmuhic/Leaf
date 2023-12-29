#pragma once

#include <vector>

#include "body.h"
#include "state.h"
#include "geometry.h"

struct Physics {
    void resolveCollisions(std::vector<Collision>& collisions, std::vector<Entity>& entities);
    void applyImpulses(Collision& c, RigidBody& a, RigidBody& b);
};
