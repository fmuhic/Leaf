#pragma once

#include "dynamic_tree.h"
#include "dynamic_stack.h"

class BroadPhase {
    public:
    BroadPhase();
    ~BroadPhase();

    i32 createBox(const AABB& box, i32 bodyId);
    void removeBox(i32 boxId);
    void moveBox(i32 boxId, AABB& newBox, glm::vec3 displacement);
    void update(DynamicStack<i32>& movedBodies);

    private:
    DynamicTree *tree;
    // Replace with set, for better deduplication
    DynamicStack<i32> *moves;
};
