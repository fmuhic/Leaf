#pragma once

#include <map>
#include <vector>
#include "types.h"
#include "body.h"
#include "broad_phase.h"
#include "narrow_phase.h"
#include "physics.h"

class Leaf {
    public:

    Leaf();
    ~Leaf();

    void step(f32 dt);
    i32 createBox(BodyConfig config);

    RigidBody* getBody(i32 id);
    void destroyBody(i32 id);

    void debugTree(std::vector<std::pair<AABB, i32>>& boxes);
    std::map<CollisionKey, Collision>* debugCollisions();

    // public for debugging
    std::map<CollisionKey, Collision> collisions = std::map<CollisionKey, Collision>();
    // Replace this with array based linked lists
    std::vector<RigidBody> bodies;

    private:

    void updateCandidates();
    i32 findFreeBody();

    BroadPhase* broadPhase;
    NarrowPhase* narrowPhase;
    Physics* physics;

    std::vector<i32> moves;
    std::vector<i32> candidatesStack;
};

