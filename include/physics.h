#pragma once

#include <vector>

#include "types.h"
#include "const.h"
#include "body.h"
#include "geometry.h"

struct Physics {
    void resolveCollisions(std::vector<Collision>& collisions, std::vector<Entity>& entities, f32 dtInv);

    private:

    void applyNormalImpulse(Collision& c, RigidBody& a, RigidBody& b, f32 dtInv);
    void applyFrictionImpulse(Collision& c, RigidBody& a, RigidBody& b);

    i32 correctionCount = 10;
    f32 positionCorrectionFactor = 0.2f;
    f32 allowedPenetration = 0.01f;
};
