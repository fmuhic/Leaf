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

void DynamicTree::removeBox(i32 boxId) {
    assert(nodes[boxId].isLeaf());
    removeLeaf(boxId);
    freeNode(boxId);
}

bool DynamicTree::moveBox(i32 boxId, AABB& newBox, glm::vec3 displacement) {
    assert(boxId > 0 && boxId < capacity);
	assert(nodes[boxId].isLeaf());

	AABB fatBox = newBox.fatten(fattenAmount);

	// Predict AABB movement
    glm::vec3 d = predictionMultiplier * displacement;

	if (d.x < 0.0f)
		fatBox.bottomLeft.x += d.x;
	else
		fatBox.topRight.x += d.x;

	if (d.y < 0.0f)
		fatBox.bottomLeft.y += d.y;
	else
		fatBox.topRight.y += d.y;

	AABB& treeBox = nodes[boxId].box;
	if (treeBox.contains(newBox))
	{
		// The tree AABB still contains the object, but it might be too large.
		// Perhaps the object was moving fast but has since gone to sleep.
		// The huge AABB is larger than the new fat AABB.
		AABB hugeBox = fatBox.fatten(4.0f * fattenAmount);

		if (hugeBox.contains(treeBox))
		{
			// The tree AABB contains the object AABB and the tree AABB is
			// not too large. No tree update needed.
			return false;
		}

		// Otherwise the tree AABB is huge and needs to be shrunk
	}

	removeLeaf(boxId);
	nodes[boxId].box = fatBox;
	insertLeaf(boxId);

	return true;
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

        // Cost of attaching new parent at this index
        f32 cost = 2.0f * combinedPerimiter;
        // Cost of going deeper into the tree
        f32 inheritedCost = 2.0f * (combinedPerimiter - perimiter);

        f32 leftCost;
        f32 leftCombinedCost = leafBox.merge(nodes[leftChild].box).perimiter();
        if (nodes[leftChild].isLeaf()) {
            leftCost = leftCombinedCost + inheritedCost;
        } else {
            f32 oldCost = nodes[leftChild].box.perimiter();
            leftCost = (leftCombinedCost - oldCost) + inheritedCost;
        }

        f32 rightCost;
        f32 rightCombinedCost = leafBox.merge(nodes[rightChild].box).perimiter();
        if (nodes[rightChild].isLeaf()) {
            rightCost = rightCombinedCost + inheritedCost;
        } else {
            f32 oldCost = nodes[rightChild].box.perimiter();
            rightCost = (rightCombinedCost - oldCost) + inheritedCost;
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

    i32 bestsibling = index;

	i32 oldParent = nodes[bestsibling].parent;
	i32 newParent = createNode();
	nodes[newParent].parent = oldParent;
	nodes[newParent].box = leafBox.merge(nodes[bestsibling].box);
	// nodes[newParent].height = nodes[bestsibling].height + 1;

    if (bestsibling == root) {
		root = newParent;
    } else {
        if (nodes[oldParent].leftChild == bestsibling)
			nodes[oldParent].leftChild = newParent;
        else
			nodes[oldParent].rightChild = newParent;
    }

    nodes[newParent].leftChild = bestsibling;
    nodes[newParent].rightChild = leafId;
    nodes[bestsibling].parent = newParent;
    nodes[leafId].parent = newParent;

    index = nodes[leafId].parent;
    while (index != NULL_NODE) {
        // Rebalance tree first
        
        i32 leftChild = nodes[index].leftChild;
        i32 rightChild = nodes[index].rightChild;

        nodes[index].box = nodes[leftChild].box.merge(nodes[rightChild].box);
        index = nodes[index].parent;
    }
}

void DynamicTree::removeLeaf(i32 boxId) {
    if (boxId == root) {
        root = NULL_NODE;
        return;
    }

    i32 parent = nodes[boxId].parent;
    i32 grandParent = nodes[parent].parent;
    i32 sibling;

    if (nodes[parent].leftChild == boxId)
        sibling = nodes[parent].rightChild;
    else 
        sibling = nodes[parent].leftChild;

    if (grandParent == NULL_NODE) {
        root = sibling;
    }
    else {
        if (nodes[grandParent].leftChild == parent)
            nodes[grandParent].leftChild = sibling;
        else
            nodes[grandParent].rightChild = sibling;

        i32 index = grandParent;
        while (index != NULL_NODE) {
            // Rebalance tree first
            
            i32 leftChild = nodes[index].leftChild;
            i32 rightChild = nodes[index].rightChild;

            nodes[index].box = nodes[leftChild].box.merge(nodes[rightChild].box);
            index = nodes[index].parent;
        }
    }

    nodes[sibling].parent = grandParent;
    freeNode(parent);
}

i32 DynamicTree::createNode() {
    if (freeList == NULL_NODE) {
        expandNodePool();
        assert(freeList != NULL_NODE);
    }

    i32 nodeId = freeList;
    freeList = nodes[freeList].next;
    nodes[nodeId].parent = NULL_NODE;
    ++count;

    return nodeId;
}

void DynamicTree::freeNode(i32 boxId) {
    assert(boxId >= 0 && boxId < capacity);
	assert(count > 0);
	nodes[boxId].next = freeList;
	freeList = boxId;
	--count;
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


void DynamicTree::getAll(std::vector<AABB>& boxes) {
    boxes.clear();
    std::vector<i32> stack;
    stack.push_back(root);

    while (!stack.empty()) {
        Node& node = nodes[stack.back()];
        stack.pop_back();

        if (node.isLeaf()) {
            boxes.push_back(node.box);
        }
        else {
            stack.push_back(node.leftChild);
            stack.push_back(node.rightChild);
        }
    }
}
