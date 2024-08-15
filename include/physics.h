#pragma once

#include <vector>
#include <map>


#include "types.h"
#include "body.h"
#include "collision.h"

struct Physics {
    void resolveCollisions(std::map<CollisionKey, Collision>& collisions, std::vector<RigidBody>& bodies, f32 dtInv);

    private:

    void prepareContacts(Collision& collision, RigidBody& a, RigidBody& b, f32 dtInv);
    void applyNormalImpulse(Collision& collision, RigidBody& a, RigidBody& b);
    void applyTangentImpulse(Collision& collision, RigidBody& a, RigidBody& b);

    i32 correctionCount = 12;
    f32 positionCorrectionFactor = 0.2f;
    f32 allowedPenetration = 0.01f;
};
