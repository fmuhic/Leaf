#pragma once

#include "body.h"
#include "types.h"

struct Node {
    AABB box;
    Node* leftChild;
    Node* rightChild;
    bool isLeaf;
};

struct DynamicTree {
    DynamicTree();

    private:

    [[maybe_unused]] Node* nodes = nullptr;
    [[maybe_unused]] i32 size = 0;
};
