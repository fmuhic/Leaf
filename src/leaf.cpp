#include "leaf.h"
#include "body.h"
#include "broad_phase.h"
#include "collision.h"
#include "geometry.h"

Leaf::Leaf() {
    physics = new Physics();
    broadPhase = new BroadPhase();
    geometry = new Geometry(1000, &vertices);
}

Leaf::~Leaf() {
    delete physics;
    delete broadPhase;
}

RigidBody* Leaf::getBody(i32 id) {
    return &bodies[id];
}

void Leaf::step(f32 dt) {
    for (i32 i = bodies.getFirst(); i != ObjectPool<RigidBody>::END; i = bodies.next(i)) {
        RigidBody& body = bodies[i];
        body.update(vertices);
        body.updateVelocity(dt);
    }

    updateCandidates();
    geometry->narrowPhase(candidatesPool, bodies);

    f32 dtInv = dt > 0.0f ? 1.0f / dt : 0.0f;

    physics->resolveCollisions(candidatesPool, bodies, dtInv);

    for (i32 i = bodies.getFirst(); i != ObjectPool<RigidBody>::END; i = bodies.next(i)) {
        RigidBody& body = bodies[i];
        body.updatePosition(dt);
        broadPhase->moveBox(body.treeId, body.aabb, body.position - body.prevPosition);
    }
}

void Leaf::updateCandidates() {
    moves.clear();
    broadPhase->update(moves);

    while (!moves.isEmpty()) {
        i32 aId = moves.pop();
        RigidBody& bodyA = bodies[aId];
        candidatesTempStack.clear();
        broadPhase->query(bodyA.aabb, candidatesTempStack);

        // Clear all candidates
        for (auto it = candidatesPool.cbegin(); it != candidatesPool.cend();) {
            if (it->first.first == aId || it->first.second == aId)
                candidatesPool.erase(it++);
            else
                ++it;
        }

        while (!candidatesTempStack.isEmpty()) {
            i32 bId = candidatesTempStack.pop();
            if (aId != bId)
                candidatesPool.insert(CollisionPair(CollisionKey(aId, bId), Collision()));
        }
    }
}

i32 Leaf::createBox(BodyConfig config, UserData data) {
    i32 bodyId = bodies.reserve();
    RigidBody& body = bodies[bodyId];
    body = RigidBody(config, GeometryType::BOX, data);
    body.verticesId = vertices.reserve(boxVertexCount);
    body.treeId = broadPhase->createBox(body.aabb, bodyId);
    bodies.debug("Bodies Add");
    vertices.debug("Vertices Add");

    return bodyId;
}

void Leaf::destroyBody(i32 id) {
    RigidBody body = bodies[id];
    vertices.free(body.verticesId);
    bodies.free(id);
    broadPhase->removeBox(body.treeId);
    bodies.debug("Bodies Remove");
    vertices.debug("Vertices Remove");

}

void Leaf::debugTree(std::vector<std::pair<AABB, i32>>& boxes) {
    broadPhase->debugTree(boxes);
}
