#include <cassert>
#include <cstring>
#include "dynamic_tree.h"
#include "leaf_math.h"

enum class TreeRotation {
    NONE,
    ROTATE_BF,
    ROTATE_BG,
    ROTATE_CD,
    ROTATE_CE
};

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
void DynamicTree::checkIntersections(AABB& box, std::vector<i32>& candidates) {
    candidates.clear();
    std::vector<i32> stack;
    stack.push_back(root);

    while (!stack.empty()) {
        i32 index = stack.back();
        Node& node = nodes[index];
        stack.pop_back();

        if (!box.overlaps(node.box))
            continue;

        if (node.isLeaf()) {
            candidates.push_back(nodes[index].data.entityId);
        } else {
            stack.push_back(node.leftChild);
            stack.push_back(node.rightChild);
        }
    }

}

i32 DynamicTree::createBox(const AABB &box, UserData data) {
    i32 boxId = createNode();
    nodes[boxId].box = box.fatten(fattenAmount);
    nodes[boxId].data = data;
    insertLeaf(boxId);
    return boxId;
}

void DynamicTree::removeBox(i32 boxId) {
    assert(nodes[boxId].isLeaf());
    removeLeaf(boxId);
    freeNode(boxId);
}

bool DynamicTree::moveBox(i32 boxId, AABB& newBox, glm::vec3 displacement) {
    assert(boxId >= 0 && boxId < capacity);
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
            leftCost = leftCombinedCost - oldCost + inheritedCost;
        }

        f32 rightCost;
        f32 rightCombinedCost = leafBox.merge(nodes[rightChild].box).perimiter();
        if (nodes[rightChild].isLeaf()) {
            rightCost = rightCombinedCost + inheritedCost;
        } else {
            f32 oldCost = nodes[rightChild].box.perimiter();
            rightCost = rightCombinedCost - oldCost + inheritedCost;
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
	nodes[newParent].height = nodes[bestsibling].height + 1;

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
        i32 leftChild = nodes[index].leftChild;
        i32 rightChild = nodes[index].rightChild;

        nodes[index].height = max(nodes[leftChild].height, nodes[rightChild].height) + 1;
        nodes[index].box = nodes[leftChild].box.merge(nodes[rightChild].box);

        balance(index);

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
            i32 leftChild = nodes[index].leftChild;
            i32 rightChild = nodes[index].rightChild;

            nodes[index].height = max(nodes[leftChild].height, nodes[rightChild].height) + 1;
            nodes[index].box = nodes[leftChild].box.merge(nodes[rightChild].box);

            index = nodes[index].parent;
        }
    }

    nodes[sibling].parent = grandParent;
    freeNode(parent);
}

//...........
//....A......  
//.../.\.....
//..B...C ...
//./.\./.\...
//.D.E.F.G...
//...........
void DynamicTree::balance(i32 indexA) {
    assert(indexA != NULL_NODE);
    Node& A = nodes[indexA];

    if (A.isLeaf() || A.height < 2) {
        return;
    }

    i32 indexB = nodes[indexA].leftChild;
    i32 indexC = nodes[indexA].rightChild;
    Node &B = nodes[indexB];
    Node &C = nodes[indexC];

    if (B.isLeaf()) {
        assert(B.height == 0);
        assert(C.height > 0 && "C has to be internal node");

        i32 indexF = nodes[indexC].leftChild;
        i32 indexG = nodes[indexC].rightChild;
        Node &F = nodes[indexF];
        Node &G = nodes[indexG];
        
        AABB boxBF = B.box.merge(F.box);
        AABB boxBG = B.box.merge(G.box);

        f32 costC = C.box.perimiter(); 
        f32 costBF = boxBG.perimiter();
        f32 costBG = boxBF.perimiter();

        if (costC <= costBF && costC <= costBG) {
            // Rotation will not imporove tree
            return;
        }

        if (costBF < costBG) {
            // Rotate B <-> F
            A.leftChild = indexF;
            F.parent = indexA;

            C.leftChild = indexB;
            B.parent = indexC;

            C.box = boxBG;

            C.height = max(B.height, G.height) + 1;
			A.height = max(C.height, F.height) + 1;
        } else {
            // Rotate B <-> G
            A.leftChild = indexG;
            G.parent = indexA;

            C.rightChild = indexB;
            B.parent = indexC;

            C.box = boxBF;

            C.height = max(F.height, B.height) + 1;
			A.height = max(G.height, C.height) + 1;
        }
    }
    else if (C.isLeaf()) {
        assert(C.height == 0);
        assert(B.height > 0 && "B has to be internal node");

        i32 indexD = nodes[indexB].leftChild;
        i32 indexE = nodes[indexB].rightChild;
        Node &D = nodes[indexD];
        Node &E = nodes[indexE];
        
        AABB boxCD = C.box.merge(D.box);
        AABB boxCE = C.box.merge(E.box);

        f32 costB = B.box.perimiter(); 
        f32 costCD = boxCE.perimiter();
        f32 costCE = boxCD.perimiter();

        if (costB <= costCD && costB <= costCE) {
            // Rotation will not imporove tree
            return;
        }

        if (costCD < costCE) {
            // Rotate C <-> D
            A.rightChild = indexD;
            D.parent = indexA;

            B.leftChild = indexC;
            C.parent = indexB;

            B.box = boxCE;

            B.height = max(C.height, E.height) + 1;
			A.height = max(B.height, D.height) + 1;
        } else {
            // Rotate C <-> E
            A.rightChild = indexE;
            E.parent = indexA;

            B.rightChild = indexC;
            C.parent = indexB;

            B.box = boxCD;

            B.height = max(D.height, C.height) + 1;
			A.height = max(B.height, E.height) + 1;
        }
    }
    else {
        // Both children are internal nodes
        i32 indexD = nodes[indexB].leftChild;
        i32 indexE = nodes[indexB].rightChild;
        i32 indexF = nodes[indexC].leftChild;
        i32 indexG = nodes[indexC].rightChild;

        Node &D = nodes[indexD];
        Node &E = nodes[indexE];
        Node &F = nodes[indexF];
        Node &G = nodes[indexG];

        f32 costB = B.box.perimiter();
        f32 costC = C.box.perimiter();
        f32 costBC = costB + costC;
        TreeRotation bestRotation = TreeRotation::NONE;
        f32 bestCost = costBC;

        // Check B <=> F rotation
		AABB boxBG = B.box.merge(G.box);
		f32 costBF = costB + boxBG.perimiter();
		if (costBF < bestCost) {
			bestCost = costBF;
			bestRotation = TreeRotation::ROTATE_BF;
		}

        // Check B <=> G rotation
		AABB boxBF = B.box.merge(F.box);
		f32 costBG = costB + boxBF.perimiter();
		if (costBG < bestCost) {
			bestCost = costBG;
			bestRotation = TreeRotation::ROTATE_BG;
		}

        // Check C <=> D rotation
		AABB boxCE = C.box.merge(E.box);
		f32 costCD = costC + boxCE.perimiter();
		if (costCD < bestCost) {
			bestCost = costCD;
			bestRotation = TreeRotation::ROTATE_CD;
		}

        // Check C <=> E rotation
		AABB boxCD = C.box.merge(D.box);
		f32 costCE = costC + boxCD.perimiter();
		if (costCE < bestCost) {
			bestRotation = TreeRotation::ROTATE_CE;
        }

        switch (bestRotation) {
            case TreeRotation::NONE:
				break;

            case TreeRotation::ROTATE_BF:
                A.leftChild = indexF;
                F.parent = indexA;

                C.leftChild = indexB;
                B.parent = indexC;

                C.box = boxBG;

                C.height = max(B.height, G.height) + 1;
                A.height = max(C.height, F.height) + 1;
				break;

            case TreeRotation::ROTATE_BG:
                A.leftChild = indexG;
                G.parent = indexA;

                C.rightChild = indexB;
                B.parent = indexC;

                C.box = boxBF;

                C.height = max(F.height, B.height) + 1;
                A.height = max(G.height, C.height) + 1;
				break;

            case TreeRotation::ROTATE_CD:
                A.rightChild = indexD;
                D.parent = indexA;

                B.leftChild = indexC;
                C.parent = indexB;

                B.box = boxCE;

                B.height = max(C.height, E.height) + 1;
                A.height = max(B.height, D.height) + 1;
				break;

            case TreeRotation::ROTATE_CE:
                A.rightChild = indexE;
                E.parent = indexA;

                B.rightChild = indexC;
                C.parent = indexB;

                B.box = boxCD;

                B.height = max(D.height, C.height) + 1;
                A.height = max(B.height, E.height) + 1;
				break;

			default:
				break;
		}

    }
}

i32 DynamicTree::createNode() {
    if (freeList == NULL_NODE) {
        expandNodePool();
        assert(freeList != NULL_NODE);
    }

    i32 nodeId = freeList;
    freeList = nodes[freeList].next;
    nodes[nodeId].parent = NULL_NODE;
    nodes[nodeId].height = 0;
    ++count;

    return nodeId;
}

void DynamicTree::freeNode(i32 boxId) {
    assert(boxId >= 0 && boxId < capacity);
	assert(count > 0);
	nodes[boxId].next = freeList;
	nodes[boxId].height = -1;
    nodes[boxId].data = UserData(-1);
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


void DynamicTree::getAll(std::vector<std::pair<AABB, i32>>& boxes) {
    boxes.clear();
    std::vector<i32> stack;
    stack.push_back(root);

    while (!stack.empty()) {
        Node& node = nodes[stack.back()];
        stack.pop_back();
        boxes.push_back(std::pair(node.box, node.height));

        if (!node.isLeaf()) {
            stack.push_back(node.leftChild);
            stack.push_back(node.rightChild);
        }
    }
}

f32 DynamicTree::quality() {
    if (root == NULL_NODE)
        return 0.0f;

    std::vector<i32> stack;
    stack.push_back(root);
    f32 cost = 0.0f;

    while (!stack.empty()) {
        Node& node = nodes[stack.back()];
        stack.pop_back();

        if (!node.isLeaf()) {
            cost += node.box.perimiter();

            stack.push_back(node.leftChild);
            stack.push_back(node.rightChild);
        }
    }

    return cost / nodes[root].box.perimiter();
}

i32 DynamicTree::height() {
    return nodes[root].height;
}
