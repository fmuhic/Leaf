#include "broad_phase.h"
#include <algorithm>

BroadPhase::BroadPhase(DynamicTree* dynamicTree) {
    tree = dynamicTree;
    moves = new std::vector<i32>();
}

BroadPhase::~BroadPhase() {
    delete tree;
    delete moves;
}

i32 BroadPhase::createBox(const AABB& box, i32 bodyId) {
    //refactor this to only use bodyId
    TreeData data(bodyId, bodyId);
    i32 id = tree->createBox(box, data);
    if(std::find(moves->begin(), moves->end(), id) == moves->end()) {
        moves->push_back(id);
    }
    return id;
}

void BroadPhase::removeBox(i32 boxId) {
    tree->removeBox(boxId);
}

void BroadPhase::moveBox(i32 boxId, AABB& newBox, glm::vec3 displacement) {
    bool moved = tree->moveBox(boxId, newBox, displacement);

    if(moved && std::find(moves->begin(), moves->end(), boxId) == moves->end()) {
        moves->push_back(boxId);
    }
}

void BroadPhase::update(std::vector<i32>& movedBodies) {
    for (i32 mv: *moves) {
        TreeData data = tree->getData(mv);
        movedBodies.push_back(data.entityId);
    }
    moves->clear();
}

void BroadPhase::query(AABB& box, std::vector<i32>& candidates) {
    tree->checkIntersections(box, candidates);
}

void BroadPhase::debugTree(std::vector<std::pair<AABB, i32>>& boxes) {
    tree->getAll(boxes);
}

