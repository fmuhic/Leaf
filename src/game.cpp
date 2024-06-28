#include "game.h"
#include "body.h"
#include "geometry.h"
#include "physics.h"
#include "types.h"

Game::Game(i32 maxEntityCount) {
    entities.reserve(maxEntityCount);
    for (i32 i = 0; i < maxEntityCount; ++i)
        entities.push_back(Entity{});

    geometry = new Geometry(maxEntityCount);
    physics = new Physics();
}

Game::~Game() {
    delete geometry;
    delete physics;
    delete example;
}

void Game::reset() {
    geometry->reset();
    for (auto &e: entities) 
        e.destroy();
}

void Game::changeScene(Example *newExample) {
    reset();
    delete example;
    example = newExample;
    example->setup(entities);
    for (auto& e: entities) {
        if (!e.isAlive)
            continue;

        i32 treeId = geometry->dynamicTree->createBox(e.body.aabb);
        e.treeId = treeId;
    }
}

void Game::update(f32 dt, f32 elapsed, MouseInput &mInput) {
    processInput(mInput);
    updateLogic(elapsed);

    for (auto &e: entities) {
        if (!e.isAlive)
            continue;

        e.body.updateVelocity(dt);
        bool destroyed = e.despawnIfOutOfBounds();
        if (destroyed)
            geometry->dynamicTree->removeBox(e.treeId);
    }

    geometry->broadPhase(entities);
    geometry->narrowPhase(entities);

    f32 dtInv = dt > 0.0f ? 1.0f / dt : 0.0f;

    physics->resolveCollisions(geometry->collisions, entities, dtInv);

    for (auto &e: entities) {
        if (!e.isAlive)
            continue;

        e.body.updatePosition(dt);
        geometry->dynamicTree->moveBox(e.treeId, e.body.aabb, e.body.position - e.body.prevPosition);
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
        e->treeId = geometry->dynamicTree->createBox(e->body.aabb);
    }
}

Entity* Game::findFreeEntity() {
    for (auto &e: entities) {
        if (!e.isAlive)
            return &e;
    }
    return nullptr;
}
