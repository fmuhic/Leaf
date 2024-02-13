#include "game.h"
#include "body.h"
#include "geometry.h"
#include "helpers.h"
#include "physics.h"
#include "types.h"

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
    delete example;
}

void Game::reset() {
    for (auto &e: entities) 
        e.destroy();
}

void Game::changeScene(Example *newExample) {
    reset();
    delete example;
    example = newExample;
    example->setup(entities);
}

void Game::update(f32 dt, f32 elapsed, MouseInput &mInput) {
    processInput(mInput);
    updateLogic(elapsed);

    for (auto &e: entities) {
        if (!e.isAlive)
            continue;

        e.body.updateVelocity(dt);
        e.despawnIfOutOfBounds();
    }

    geometry->broadPhase(entities);
    geometry->narrowPhase(entities);

    f32 dtInv = dt > 0.0f ? 1.0f / dt : 0.0f;

    physics->resolveCollisions(geometry->collisions, entities, dtInv);

    for (auto &e: entities) {
        if (!e.isAlive)
            continue;

        e.body.updatePosition(dt);
    }
}

void Game::updateLogic(f32 elapsed) {
    example->update(entities, elapsed);
}

void Game::processInput(MouseInput &mInput) {
    if (mInput.clicked(MouseButton::LEFT)) {
        Entity* e = findFreeEntity();
        if (e == nullptr)
            return;

        e->activate(glm::vec3(mInput.position.x, mInput.position.y, 0.0f));
    }
}

Entity* Game::findFreeEntity() {
    for (auto &e: entities) {
        if (!e.isAlive)
            return &e;
    }
    return nullptr;
}
