#pragma once

#include "body.h"
#include "const.h"
#include "types.h"

#include "glm/ext/vector_float3.hpp"
#include <vector>

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

// Based on Box2D DynamicTree
struct DynamicTree {
    DynamicTree();
    ~DynamicTree();

    i32 createBox(const AABB& box);
    void removeBox(i32 boxId);
    bool moveBox(i32 boxId, AABB& newBox, glm::vec3 displacement);
    void getAll(std::vector<AABB>& boxes);

    private:

    void freeNode(i32 boxId);
    i32 createNode();
    void expandNodePool();
    void insertLeaf(i32 boxId);
    void removeLeaf(i32 boxId);

    Node* nodes = nullptr;
    i32 count = 0;
    i32 capacity = INIT_TREE_CAPACITY;
    i32 freeList = 0;
    i32 root = NULL_NODE;

    f32 fattenAmount = 0.1f * UNITS_PER_METER;
    f32 predictionMultiplier = 4.0f;
};
