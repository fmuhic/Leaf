#pragma once

#include <vector>
#include "dynamic_tree.h"

class BroadPhase {
    public:
    BroadPhase();
    ~BroadPhase();

    i32 createBox(const AABB& box, i32 bodyId);
    void removeBox(i32 boxId);
    void moveBox(i32 boxId, AABB& newBox, glm::vec3 displacement);
    void update(std::vector<i32>& movedBodies);
    void query(i32 boxId, std::vector<i32>& candidates);

    void debugTree(std::vector<std::pair<AABB, i32>>& boxes);

    private:

    DynamicTree *tree;
    std::vector<i32> treeMoves;
};

