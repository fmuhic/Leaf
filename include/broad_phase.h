#pragma once

#include "dynamic_tree.h"
#include <vector>

class BroadPhase {
    public:
    BroadPhase(DynamicTree* dynamicTree);
    ~BroadPhase();

    i32 createBox(const AABB& box, i32 bodyId);
    void removeBox(i32 boxId);
    void moveBox(i32 boxId, AABB& newBox, glm::vec3 displacement);
    void update(std::vector<i32>& movedBodies);
    void query(AABB& box, std::vector<i32>& candidates);

    void debugTree(std::vector<std::pair<AABB, i32>>& boxes);

    private:
    DynamicTree *tree;
    // Replace with set, for better deduplication
    std::vector<i32> *moves;
};

