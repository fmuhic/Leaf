#pragma once

#include <vector>
#include <map>

#include "entity.h"
#include "glm/ext/vector_float3.hpp"
#include "leaf.h"

#define MAX_ENTITY_COUNT 1000

class EntitySystem {
    public:
    EntitySystem(Leaf *leaf);

    i32 createImmovableGround(
        glm::vec3 position,
        glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f),
        f32 rotation = 0.0f
    );
    i32 createDynamicBox(
        glm::vec3 position,
        glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f),
        f32 rotation = 0.0f
    );
    void destroyEntity(i32 id);
    void reset();

    std::vector<Entity>* debugEntities();
    std::map<CollisionKey, Collision>* debugCollisions();
    void debugTree(std::vector<std::pair<AABB, i32>>& boxes);

    std::vector<Entity> entities;
    Leaf* leaf;
    private:

    i32 findFreeEntity();

};

