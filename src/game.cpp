#include "game.h"
#include "body.h"
#include "broad_phase.h"
#include "geometry.h"
#include "physics.h"
#include "types.h"
#include <algorithm>

Game::Game(i32 maxEntityCount) {
    tree = new DynamicTree();
    geometry = new Geometry(tree, maxEntityCount);
    broadPhase = new BroadPhase(tree);
    physics = new Physics();

    entities.reserve(maxEntityCount);
    for (i32 i = 0; i < maxEntityCount; ++i)
        entities.push_back(Entity());
}

Game::~Game() {
    delete geometry;
    delete physics;
    delete example;
}

void Game::reset() {
    geometry->reset();
    for (auto &e: entities) {
        if (!e.isAlive) {
            assert(e.treeId == -1);
            continue;
        }
        e.destroy();
        broadPhase->removeBox(e.treeId);
        e.treeId = -1;
    }
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

        i32 treeId = broadPhase->createBox(e.body.aabb, i);
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
        if (e.despawnIfOutOfBounds()) {
            broadPhase->removeBox(e.treeId);
            e.treeId = -1;
        }
    }

    updateCandidates();
    // geometry->broadPhase(entities, candidatesPool);
    geometry->narrowPhase(entities, candidatesPool);

    f32 dtInv = dt > 0.0f ? 1.0f / dt : 0.0f;

    physics->resolveCollisions(candidatesPool, entities, dtInv);

    for (auto &e: entities) {
        if (!e.isAlive)
            continue;

        e.body.updatePosition(dt);
        broadPhase->moveBox(e.treeId, e.body.aabb, e.body.position - e.body.oldPosition);
    }
}

void Game::updateLogic([[maybe_unused]]f32 elapsed) {
    example->update(entities, elapsed);
}

void Game::processInput(MouseInput &mInput) {
    if (mInput.clicked(MouseButton::LEFT)) {
        for (i32 i = 0; i < (i32) entities.size(); ++i) {
            Entity& e = entities[i];
            if (e.isAlive) {
                continue;
            }

            e.activate(glm::vec3(mInput.position.x, mInput.position.y, 0.0f));
            e.treeId = broadPhase->createBox(e.body.aabb, i);
            break;
        }
    }
}

void Game::updateCandidates() {
    moves.clear();
    broadPhase->update(moves);
    for (i32 aId: moves) {
        Entity& a = entities[aId];
        tempStack.clear();
        broadPhase->query(a.treeId, tempStack);

        // Clear all candidates containint A's id
        for (auto it = candidatesPool.cbegin(); it != candidatesPool.cend();) {
            if (it->first.first == aId || it->first.second == aId)
                candidatesPool.erase(it++);
            else
                ++it;
        }

        for (i32 bId: tempStack) {
            if (aId != bId) {
                candidatesPool.insert(
                    CollisionPair(CollisionKey(aId, bId), Collision())
                );
            }
        }
    }
}

void Game::debugTree(std::vector<std::pair<AABB, i32>>& boxes) {
    broadPhase->debugTree(boxes);
}

