#include "broad_phase.h"

BroadPhase::BroadPhase() {
    tree = new DynamicTree();
    moves = new DynamicStack<i32>();
}

BroadPhase::~BroadPhase() {
    delete tree;
    delete moves;
}

i32 BroadPhase::createBox(const AABB& box, i32 bodyId) {
    TreeData data(-1, bodyId);
    i32 id = tree->createBox(box, data);
    if (!moves->contains(id))
        moves->push(id);
    return id;
}

void BroadPhase::removeBox(i32 boxId) {
    tree->removeBox(boxId);
}

void BroadPhase::moveBox(i32 boxId, AABB& newBox, glm::vec3 displacement) {
    bool moved = tree->moveBox(boxId, newBox, displacement);
    if (moved) {
        if (!moves->contains(boxId))
            moves->push(boxId);
    }
}

void BroadPhase::update(DynamicStack<i32>& movedBodies) {
    while (!moves->isEmpty()) {
        i32 boxId = moves->pop();
        TreeData data = tree->getData(boxId);
        movedBodies.push(data.bodyId);
    }
}

void BroadPhase::query(AABB& box, DynamicStack<i32>& candidates) {
    tree->checkIntersections(box, candidates);
}

void BroadPhase::debugTree(std::vector<std::pair<AABB, i32>>& boxes) {
    tree->getAll(boxes);
}
