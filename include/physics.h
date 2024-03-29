#pragma once

#include <vector>

#include "types.h"
#include "const.h"
#include "body.h"
#include "geometry.h"

struct Physics {
    void resolveCollisions(std::map<CollisionKey, Collision>& collisions, std::vector<Entity>& entities, f32 dtInv);

    private:

    void prepareContacts(Collision& collision, RigidBody& a, RigidBody& b, f32 dtInv);
    void applyNormalImpulse(Collision& collision, RigidBody& a, RigidBody& b);
    void applyTangentImpulse(Collision& collision, RigidBody& a, RigidBody& b);

    i32 correctionCount = 5;
    f32 positionCorrectionFactor = 0.2f;
    f32 allowedPenetration = 0.01f;
};
