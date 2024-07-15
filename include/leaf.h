#pragma once

#include "body.h"
#include "types.h"
#include "object_pool.h"

class Leaf {
    public:

    Leaf();
    ~Leaf();

    void step(f32 dt);
    
    private:

    ObjectPool<RigidBody> bodyPool;
};
