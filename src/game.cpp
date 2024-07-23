#include "game.h"
#include "body.h"
#include "dynamic_tree.h"
#include "entity_system.h"
#include "geometry.h"
#include "physics.h"
#include "types.h"

Game::Game(i32 maxEntityCount) {
    entities.reserve(maxEntityCount);
    for (i32 i = 0; i < maxEntityCount; ++i)
        entities.push_back(Entity{});

    geometry = new Geometry(maxEntityCount);
    physics = new Physics();
    leaf = new Leaf();
    entitySystem = new EntitySystem(leaf);
    i32 id = entitySystem->createImmovableGround();
    ids.push(id);
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
    for (i32 i = 0; i < (i32) entities.size(); ++i) {
        Entity& e = entities[i];
        if (!e.isAlive)
            continue;

        i32 treeId = geometry->dynamicTree->createBox(e.body.aabb, TreeData(i));
        e.treeId = treeId;
    }
}

void Game::update(f32 dt, f32 elapsed, MouseInput &mInput) {
    leaf->step(dt);
    processInput(mInput);
    updateLogic(elapsed);

    for (auto &e: entities) {
        if (!e.isAlive)
            continue;

        e.body.updateVelocity(dt);
        bool destroyed = e.despawnIfOutOfBounds();
        if (destroyed) {
            geometry->dynamicTree->removeBox(e.treeId);
            e.treeId = -1;
        }
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
        i32 id = entitySystem->createDynamicBox(glm::vec3(mInput.position.x, mInput.position.y, 0.0f));
        ids.push(id);
        for (i32 i = 0; i < (i32) entities.size(); ++i) {
            Entity& e = entities[i];
            if (e.isAlive) {
                continue;
            }

            e.activate(glm::vec3(mInput.position.x, mInput.position.y, 0.0f));
            e.treeId = geometry->dynamicTree->createBox(e.body.aabb, TreeData(i));
            break;
        }
    }
    if (mInput.clicked(MouseButton::RIGHT)) {
        entitySystem->destroyEntity(ids.top());
        ids.pop();
    }
}
