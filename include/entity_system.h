#pragma once

#include "glm/ext/vector_float3.hpp"
#include "leaf.h"
#include "object_pool.h"
class EntitySystem {
    public:
    EntitySystem(Leaf *leaf);

    i32 createImmovableGround();
    // Todo(Fudo) This need to be more flexible
    i32 createDynamicBox(glm::vec3 position);
    void destroyEntity(i32 id);

    ObjectPool<Entity> entities;

    private:
    Leaf* leaf;

};
