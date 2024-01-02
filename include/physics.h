#pragma once

#include <vector>

#include "types.h"
#include "const.h"
#include "body.h"
#include "geometry.h"

struct Physics {
    void resolveCollisions(std::vector<Collision>& collisions, std::vector<Entity>& entities);
    void applyImpulses(Collision& c, RigidBody& a, RigidBody& b);
};
