#pragma once

#include "body.h"
#include "const.h"
#include "types.h"

#include "glm/ext/vector_float3.hpp"
#include <utility>
#include <vector>

#define NULL_NODE (-1)
#define INIT_TREE_CAPACITY 4

struct TreeData {
    TreeData(i32 id): entityId(id) {}
    TreeData(): TreeData(-1) {}

    i32 entityId;
};

struct Node {
    AABB box;
    i32 leftChild = NULL_NODE;
    i32 rightChild = NULL_NODE;
    i32 height = -1;

    // In free list it's next, in tree it's parent
    union {
        i32 parent = NULL_NODE;
        i32 next;
    };

    TreeData data;

    bool isLeaf();
};

// Based on Box2D DynamicTree
struct DynamicTree {
    DynamicTree();
    ~DynamicTree();

    void checkIntersections(AABB& box, std::vector<i32>& candidates);
    i32 createBox(const AABB& box, TreeData data);
    void removeBox(i32 boxId);
    bool moveBox(i32 boxId, AABB& newBox, glm::vec3 displacement);
    void getAll(std::vector<std::pair<AABB, i32>>& boxes);
    f32 quality();
    i32 height();

    private:

    void freeNode(i32 boxId);
    i32 createNode();
    void expandNodePool();
    void insertLeaf(i32 boxId);
    void removeLeaf(i32 boxId);
    void balance(i32 indexA);

    Node* nodes = nullptr;
    i32 count = 0;
    i32 capacity = INIT_TREE_CAPACITY;
    i32 freeList = 0;
    i32 root = NULL_NODE;

    f32 fattenAmount = 0.1f * UNITS_PER_METER;
    f32 predictionMultiplier = 4.0f;
};
