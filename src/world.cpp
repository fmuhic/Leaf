#include "world.h"
#include "body.h"
#include "geometry.h"
#include "physics.h"

World::World(i32 maxEntityCount) {
    geometry = new Geometry(maxEntityCount);
    physics = new Physics();

    entities.reserve(maxEntityCount);
    for (i32 i = 0; i < maxEntityCount; ++i)
        entities.push_back(Entity{});
}

World::~World() {
    delete geometry;
    delete physics;
}

void World::update(f32 dt, [[maybe_unused]] KeyboardInput &kInput, MouseInput &mInput) {
    processInput(mInput);
    for (auto &e: entities) {
        if (!e.isAlive)
            continue;

        e.body.step(dt);
        e.despawnIfOutOfBounds();
    }

    geometry->broadPhase(&entities);
    geometry->narrowPhase(&entities);
    physics->resolveCollisions(geometry->collisions, entities);
}

void World::processInput(MouseInput &mInput) {
    if (mInput.leftClickClicked) {
        Entity* e = findFreeEntity();
        if (e == nullptr)
            return;

        e->body.reset();
        e->isAlive = true;
        e->body.position = glm::vec3(mInput.position.x, mInput.position.y, 0.0f);
    }
}

Entity* World::findFreeEntity() {
    for (auto &e: entities) 
        if (!e.isAlive)
            return &e;
    return nullptr;
}

void World::createStackingScene() {
    Entity *e = findFreeEntity();
    if (e == nullptr)
        return;

    // Immovable ground
    e->body = RigidBody(
        BodyType::RECTANGLE,
        glm::vec3(25.0f, 1.0f, 1.0f),
        glm::vec3(0.0f, -8.0f, 0.0f),
        true,
        0.55,
        0.35f,
        0.5f
    );
    e->isAlive = true;
    e->color = glm::vec3(1.0f, 1.0f, 1.0f);
}
