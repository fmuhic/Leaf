#pragma once

#include "body.h"
#include "broad_phase.h"
#include "collision.h"
#include "dynamic_stack.h"
#include "geometry.h"
#include "physics.h"
#include "glm/fwd.hpp"
#include "types.h"
#include "object_pool.h"
#include "list_pool.h"
#include <map>

class Leaf {
    public:

    Leaf();
    ~Leaf();

    void step(f32 dt);
    i32 createBox(BodyConfig config, UserData userData);

    RigidBody* getBody(i32 id);
    void destroyBody(i32 id);
    void debugTree(std::vector<std::pair<AABB, i32>>& boxes);

    std::map<CollisionKey, Collision> candidatesPool = std::map<CollisionKey, Collision>();

    private:

    void updateCandidates();

    const i32 boxVertexCount = 4;

    BroadPhase* broadPhase;
    Geometry* geometry;
    Physics* physics;

    ObjectPool<RigidBody> bodies = ObjectPool<RigidBody>(2);
    ListPool<glm::vec3> vertices = ListPool<glm::vec3>(8);
    DynamicStack<i32> moves = DynamicStack<i32>(8);
    // ListPool<Collision> candidatesPool = ListPool<Collision>(8);
    DynamicStack<i32> candidatesTempStack = DynamicStack<i32>(8);
};
