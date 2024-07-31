#pragma once

#include <vector>

#include "types.h"
#include "leaf_math.h"
#include "body.h"
#include "geometry.h"
#include "object_pool.h"

struct Physics {
    void resolveCollisions(std::map<CollisionKey, Collision>& collisions, std::vector<Entity>& entities, f32 dtInv);
    void resolveCollisions(std::map<CollisionKey, Collision>& candidates, ObjectPool<RigidBody>& bodies, f32 dtInv);

    private:

    void prepareContacts(Collision& collision, RigidBody& a, RigidBody& b, f32 dtInv);
    void applyNormalImpulse(Collision& collision, RigidBody& a, RigidBody& b);
    void applyTangentImpulse(Collision& collision, RigidBody& a, RigidBody& b);

    // revert this
    i32 correctionCount = 10;
    f32 positionCorrectionFactor = 0.2f;
    f32 allowedPenetration = 0.01f;
};
