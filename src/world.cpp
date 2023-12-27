#include "world.h"
#include "geometry.h"

World::World(i32 maxEntityCount) {
    geometry = new Geometry();
    entities.reserve(maxEntityCount);
    for (i32 i = 0; i < maxEntityCount; ++i)
        entities.push_back(Entity{});
}

World::~World() {
    delete geometry;
}

void World::update(f32 dt, [[maybe_unused]] KeyboardInput *kInput, MouseInput *mInput) {
    processInput(mInput);
    for (auto &e: entities) {
        if (!e.isAlive)
            continue;

        e.body.step(dt);
        e.despawnIfOutOfBounds();
    }

    geometry->broadPhase(&entities);
    geometry->narrowPhase(&entities);
}

void World::processInput(MouseInput *mInput) {
    if (mInput->leftClickClicked) {
        Entity* e = findFreeEntity();
        if (e == nullptr)
            return;

        e->body.reset();
        e->isAlive = true;
        e->body.position = glm::vec3(mInput->position.x, mInput->position.y, 0.0f);
    }
}

Entity* World::findFreeEntity() {
    for (auto &e: entities) 
        if (!e.isAlive)
            return &e;
    return nullptr;
}
