#include "entity_system.h"
#include <iostream>

EntitySystem::EntitySystem(Leaf *leaf) {
    this->leaf = leaf;
}

i32 EntitySystem::createImmovableGround() {
    i32 id = entities.reserve();
    entities[id].isAlive = true;
    // Entity e = entities[id];
    // e = Entity();
    // e.isAlive = true;

    UserData data = {};
    data.entityId = id;
    BodyConfig config = {};
    config.position = glm::vec3(0.0f, -8.0f, 0.0f);
    config.scale = glm::vec3(25.0f, 1.0f, 1.0f);
    config.immovable = true;

    entities[id].bodyId = leaf->createBox(config, data);
    entities.debug("Entities Add Ground");

    return id;
}

i32 EntitySystem::createDynamicBox(glm::vec3 position) {
    i32 id = entities.reserve();
    entities[id].isAlive = true;
    // Entity e = entities[id];
    // e = Entity();
    // e.isAlive = true;

    UserData data = {};
    data.entityId = id;
    BodyConfig config = {};
    config.position = position;
    config.scale = glm::vec3(25.0f, 1.0f, 1.0f);

    entities[id].bodyId = leaf->createBox(config, data);
    entities.debug("Entities Add");

    return id;
}

void EntitySystem::destroyEntity(i32 id) {
    std::cout << "Removig entity " << id << "\n";
    std::cout << "Entity " << entities[id].isAlive << "\n";
    Entity e = entities[id];
    assert(e.isAlive);

    leaf->destroyBody(e.bodyId);
    entities.free(id);
    entities.debug("Entities Remove");
}
