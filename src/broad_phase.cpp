#include "broad_phase.h"

#include <algorithm>
#include "dynamic_tree.h"

BroadPhase::BroadPhase() {
    tree = new DynamicTree();
}

BroadPhase::~BroadPhase() {
    delete tree;
}

i32 BroadPhase::createBox(const AABB& box, i32 bodyId) {
    TreeData data(bodyId);
    i32 id = tree->createBox(box, data);
    if(std::find(treeMoves.begin(), treeMoves.end(), id) == treeMoves.end()) {
        treeMoves.push_back(id);
    }
    return id;
}

void BroadPhase::removeBox(i32 boxId) {
    tree->removeBox(boxId);
    treeMoves.erase(std::remove(treeMoves.begin(), treeMoves.end(), boxId), treeMoves.end());
}

void BroadPhase::moveBox(i32 boxId, AABB& newBox, glm::vec3 displacement) {
    bool moved = tree->moveBox(boxId, newBox, displacement);

    if(moved && std::find(treeMoves.begin(), treeMoves.end(), boxId) == treeMoves.end()) {
        treeMoves.push_back(boxId);
    }
}

void BroadPhase::update(std::vector<i32>& movedBodies) {
    for (i32 mv: treeMoves) {
        TreeData data = tree->getData(mv);
        movedBodies.push_back(data.bodyId);
    }
    treeMoves.clear();
}

void BroadPhase::query(i32 boxId, std::vector<i32>& candidates) {
    AABB box = tree->getBox(boxId);
    tree->checkIntersections(box, candidates);
}

void BroadPhase::debugTree(std::vector<std::pair<AABB, i32>>& boxes) {
    tree->getAll(boxes);
}

