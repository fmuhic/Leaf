#include "game.h"
#include "body.h"
#include "geometry.h"
#include "physics.h"

Game::Game(i32 maxEntityCount) {
    geometry = new Geometry(maxEntityCount);
    physics = new Physics();

    entities.reserve(maxEntityCount);
    for (i32 i = 0; i < maxEntityCount; ++i)
        entities.push_back(Entity{});
}

Game::~Game() {
    delete geometry;
    delete physics;
}

void Game::update(f32 dt, MouseInput &mInput) {
    processInput(mInput);
    for (auto &e: entities) {
        if (!e.isAlive)
            continue;

        e.body.step(dt);
        e.despawnIfOutOfBounds();
    }

    f32 dtInv = dt > 0.0f ? 1.0f / dt : 0.0f;

    geometry->broadPhase(entities);
    geometry->narrowPhase(entities);
    physics->resolveCollisions(geometry->collisions, entities, dtInv);

    for (auto &e: entities) {
        if (!e.isAlive)
            continue;

        e.body.updatePosition(dt);
    }
}

void Game::processInput(MouseInput &mInput) {
    if (mInput.clicked(MouseButton::LEFT)) {
        Entity* e = findFreeEntity();
        if (e == nullptr)
            return;

        e->body.reset();
        e->isAlive = true;
        e->body.position = glm::vec3(mInput.position.x, mInput.position.y, 0.0f);
    }
}

Entity* Game::findFreeEntity() {
    for (auto &e: entities) 
        if (!e.isAlive)
            return &e;
    return nullptr;
}

void Game::createStackingScene() {
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
