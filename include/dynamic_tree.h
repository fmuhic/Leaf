#pragma once

#include "body.h"
#include "const.h"
#include "types.h"

#define NULL_NODE (-1)
#define INIT_TREE_CAPACITY 4

struct Node {
    AABB box;
    i32 leftChild = NULL_NODE;
    i32 rightChild = NULL_NODE;

    // In free list it's next, in tree it's parent
    union {
        i32 parent = NULL_NODE;
        i32 next;
    };

    bool isLeaf();
};

struct DynamicTree {
    DynamicTree();
    ~DynamicTree();

    i32 createBox(const AABB &box);
    void removeBox(i32 boxId);

    private:

    i32 createNode();
    void expandNodePool();
    void insertLeaf(i32 boxId);

    Node* nodes = nullptr;
    i32 count = 0;
    i32 capacity = INIT_TREE_CAPACITY;
    i32 freeList = 0;
    i32 root = NULL_NODE;
    f32 fattenAmount = 0.1f * UNITS_PER_METER;
};
