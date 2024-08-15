#include "leaf.h"
#include "leaf_math.h"

Leaf::Leaf() {
    physics = new Physics();
    broadPhase = new BroadPhase();
    narrowPhase = new NarrowPhase();

    bodies.reserve(1000);
    for (i32 i = 0; i < 1000; ++i)
        bodies.push_back(RigidBody());
}

Leaf::~Leaf() {
    delete physics;
    delete broadPhase;
}

RigidBody* Leaf::getBody(i32 id) {
    return &bodies[id];
}

void Leaf::step(f32 dt) {
    for (RigidBody& b: bodies) {
        if (!b.isAlive) continue;
        b.updateVelocity(dt);
    }

    updateCandidates();
    narrowPhase->checkPairwiseCollision(bodies, collisions);

    f32 dtInv = dt > 0.0f ? 1.0f / dt : 0.0f;
    physics->resolveCollisions(collisions, bodies, dtInv);

    for (RigidBody& b: bodies) {
        if (!b.isAlive) continue;
        b.updatePosition(dt);
        broadPhase->moveBox(b.treeId, b.aabb, b.position - b.oldPosition);
    }
}

// This is quite inefficient. We need better data structure than std tree
void Leaf::updateCandidates() {
    moves.clear();
    broadPhase->update(moves);
    for (i32 aId: moves) {
        RigidBody& a = bodies[aId];
        candidatesStack.clear();
        broadPhase->query(a.treeId, candidatesStack);

        // Clear all candidates containint A's id
        for (auto it = collisions.cbegin(); it != collisions.cend();) {
            if (it->first.first == aId || it->first.second == aId)
                collisions.erase(it++);
            else
                ++it;
        }

        for (i32 bId: candidatesStack) {
            if (aId == bId) continue;
            collisions.insert(
                CollisionPair(
                    CollisionKey(min(aId, bId), max(aId, bId)),
                    Collision()
                )
            );
        }
    }
}

i32 Leaf::createBox(BodyConfig config) {
    i32 bodyId = findFreeBody();
    if (bodyId == -1) return -1;

    RigidBody& body = bodies[bodyId];
    body = RigidBody(config, GeometryType::BOX);
    body.treeId = broadPhase->createBox(body.aabb, bodyId);
    body.isAlive = true;

    return bodyId;
}

void Leaf::destroyBody(i32 id) {
    RigidBody& body = bodies[id];
    body.isAlive = false;
    broadPhase->removeBox(body.treeId);
    for (auto it = collisions.cbegin(); it != collisions.cend();) {
        if (it->first.first == id || it->first.second == id)
            collisions.erase(it++);
        else
            ++it;
    }
}

i32 Leaf::findFreeBody() {
    for (i32 i = 0; i < (i32) bodies.size(); ++i)
        if (!bodies[i].isAlive)
            return i;
    return -1;
}

void Leaf::debugTree(std::vector<std::pair<AABB, i32>>& boxes) {
    broadPhase->debugTree(boxes);
}

std::map<CollisionKey, Collision>* Leaf::debugCollisions() {
    return &collisions;
}
