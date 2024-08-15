#include "entity_system.h"
#include "glm/fwd.hpp"
#include "const.h"
#include "helpers.h"

EntitySystem::EntitySystem(Leaf *leaf) {
    this->leaf = leaf;

    entities.reserve(MAX_ENTITY_COUNT);
    for (i32 i = 0; i < MAX_ENTITY_COUNT; ++i)
        entities.push_back(Entity());
}

i32 EntitySystem::createImmovableGround(
    glm::vec3 position,
    glm::vec3 scale,
    f32 rotation
) {
    i32 id = findFreeEntity();
    if (id == -1) return -1;

    BodyConfig config = {};
    config.position = position;
    config.scale = scale;
    config.rotation = rotation;
    config.immovable = true;

    entities[id].bodyId = leaf->createBox(config);
    entities[id].isAlive = true;
    entities[id].color = glm::vec3(1.0f, 1.0f, 1.0f);

    return id;
}

i32 EntitySystem::createDynamicBox(
    glm::vec3 position,
    glm::vec3 scale,
    f32 rotation
) {
    i32 id = findFreeEntity();
    if (id == -1) return -1;

    BodyConfig config = {};
    config.position = position;
    config.scale = scale;
    config.rotation = rotation;

    entities[id].bodyId = leaf->createBox(config);
    entities[id].isAlive = true;
    entities[id].color = COLORS[pickRand(0, COLORS.size() - 1)]; 

    return id;
}

void EntitySystem::destroyEntity(i32 id) {
    Entity& e = entities[id];
    assert(e.isAlive);
    e.isAlive = false;

    leaf->destroyBody(e.bodyId);
}

void EntitySystem::reset() {
    for (i32 i = 0; i < (i32) entities.size(); ++i)
        if (entities[i].isAlive)
            destroyEntity(i);
}

i32 EntitySystem::findFreeEntity() {
    for (i32 i = 0; i < (i32) entities.size(); ++i)
        if (!entities[i].isAlive)
            return i;
    return -1;
}

std::vector<Entity>* EntitySystem::debugEntities() {
    return &entities;
}

std::map<CollisionKey, Collision>* EntitySystem::debugCollisions() {
    return leaf->debugCollisions();
}
void EntitySystem::debugTree(std::vector<std::pair<AABB, i32>>& boxes) {
    leaf->debugTree(boxes);
}
