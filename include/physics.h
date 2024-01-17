#pragma once

#include <vector>

#include "types.h"
#include "const.h"
#include "body.h"
#include "geometry.h"

struct Physics {
    void positionSolver(Collision& c, RigidBody& a, RigidBody& b);
    void resolveCollisions(std::vector<Collision>& collisions, std::vector<Entity>& entities);
    void applyNormalImpulse(Collision& c, RigidBody& a, RigidBody& b);
    void applyFrictionImpulse(Collision& c, RigidBody& a, RigidBody& b);
    void applyContactImpulse(Collision& c, RigidBody& a, RigidBody& b);
};
