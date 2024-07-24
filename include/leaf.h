#pragma once

#include "body.h"
#include "broad_phase.h"
#include "physics.h"
#include "glm/fwd.hpp"
#include "types.h"
#include "object_pool.h"
#include "list_pool.h"

class Leaf {
    public:

    Leaf();
    ~Leaf();

    void step(f32 dt);
    i32 createBox(BodyConfig config, UserData userData);
    void destroyBody(i32 id);
    
    private:

    const i32 boxVertexCount = 4;

    BroadPhase* broadPhase;
    Physics* physics;

    ObjectPool<RigidBody> bodies = ObjectPool<RigidBody>(2);
    ListPool<glm::vec3> vertices = ListPool<glm::vec3>(8);
};
