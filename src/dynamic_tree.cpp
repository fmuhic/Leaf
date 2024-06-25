#include <cassert>
#include <cstring>
#include "dynamic_tree.h"

bool Node::isLeaf() {
    return leftChild == NULL_NODE && rightChild == NULL_NODE;
}

DynamicTree::DynamicTree() {
    nodes = new Node[capacity];

    for (i32 i = 0; i < capacity - 1; ++i) {
        nodes[i].next = i + 1;
    }

    nodes[capacity - 1].next = NULL_NODE;
}

DynamicTree::~DynamicTree() {
    delete [] nodes;
}

i32 DynamicTree::createBox(const AABB &box) {
    i32 boxId = createNode();
    nodes[boxId].box = box.fatten(fattenAmount);
    insertLeaf(boxId);
    return boxId;
}

void DynamicTree::insertLeaf(i32 leafId) {
    if (root == NULL_NODE) {
        root = leafId;
        return;
    }

    AABB &leafBox = nodes[leafId].box;
    i32 index = root;
    while (!nodes[index].isLeaf()) {
        i32 leftChild = nodes[index].leftChild;
        i32 rightChild = nodes[index].rightChild;

        f32 perimiter = nodes[index].box.perimiter();
        AABB combinedBox = leafBox.merge(nodes[index].box);
        f32 combinedPerimiter = combinedBox.perimiter();

        // Cost of placing leaf as child of current parent
        f32 cost = 2.0f * combinedPerimiter;
        // Cost of going deeper into the tree
        f32 inheritedCost = 2.0f * (combinedPerimiter - perimiter);

        f32 leftCost;
        AABB leftCombinedBox = leafBox.merge(nodes[leftChild].box);
        if (nodes[leftChild].isLeaf()) {
            leftCost = leftCombinedBox.perimiter() + inheritedCost;
        } else {
            f32 oldCost = nodes[leftChild].box.perimiter();
            f32 newCost = leftCombinedBox.perimiter();
            leftCost = (newCost - oldCost) + inheritedCost;
        }

        f32 rightCost;
        AABB rightCombinedBox = leafBox.merge(nodes[rightChild].box);
        if (nodes[rightChild].isLeaf()) {
            rightCost = rightCombinedBox.perimiter() + inheritedCost;
        } else {
            f32 oldCost = nodes[rightChild].box.perimiter();
            f32 newCost = rightCombinedBox.perimiter();
            rightCost = (newCost - oldCost) + inheritedCost;
        }

		if (cost < leftCost && cost < rightCost) {
            // Going deeper into the tree is not worth it
			break;
        }

		if (leftCost < rightCost)
			index = leftChild;
		else
			index = rightChild;
    }

    i32 bestSibiling = index;

	i32 oldParent = nodes[bestSibiling].parent;
	i32 newParent = createNode();
	nodes[newParent].parent = oldParent;
	nodes[newParent].box = leafBox.merge(nodes[bestSibiling].box);
	// nodes[newParent].height = nodes[bestSibiling].height + 1;

    if (bestSibiling == root) {
		root = newParent;
    } else {
        if (nodes[oldParent].leftChild == bestSibiling)
			nodes[oldParent].leftChild = newParent;
        else
			nodes[oldParent].rightChild = newParent;
    }

    nodes[newParent].leftChild = bestSibiling;
    nodes[newParent].rightChild = leafId;
    nodes[bestSibiling].parent = newParent;
    nodes[leafId].parent = newParent;

    index = nodes[leafId].parent;
    while (index != NULL_NODE) {
        // Rebalance tree
        i32 leftChild = nodes[index].leftChild;
        i32 rightChild = nodes[index].rightChild;

        nodes[index].box = nodes[leftChild].box.merge(nodes[rightChild].box);
    }
}

void DynamicTree::removeBox([[maybe_unused]] i32 boxId) {
}

i32 DynamicTree::createNode() {
    if (freeList == NULL_NODE) {
        expandNodePool();
        assert(freeList != NULL_NODE);
    }

    i32 nodeId = freeList;
    freeList = nodes[freeList].next;

    nodes[nodeId].parent = NULL_NODE;

    return nodeId;
}

void DynamicTree::expandNodePool() {
    assert(count == capacity);

    capacity *= 2;
    Node* old = nodes; 
    nodes = new Node[capacity];
    memcpy(nodes, old, count * sizeof(Node));
    delete [] old;

    for (i32 i = count; i < capacity - 1; ++i) {
        nodes[i].next = i + 1;
    }

    nodes[capacity - 1].next = NULL_NODE;
    freeList = count;
}
