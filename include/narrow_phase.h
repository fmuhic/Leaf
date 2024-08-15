#pragma once

#include <vector>
#include <map>

#include "types.h"
#include "collision.h"
#include "body.h"

typedef std::pair<i32, i32> EdgeId;

struct EdgePoints {
    i32 count = 0;
    glm::vec3 points[2];
};

struct Edge {
    EdgeId id;
    glm::vec3 max;
    glm::vec3 first;
    glm::vec3 second;
};

struct NarrowPhase {

    void checkPairwiseCollision(std::vector<RigidBody>& bodies, std::map<CollisionKey, Collision>& collisions);

    private:

    Collision checkPlygonPolygon(RigidBody& a, RigidBody& b);
    void findContactPoints(RigidBody& a, RigidBody& b, Collision& c);
    Edge findContactEdge(glm::vec3* vertices, i32 count, glm::vec3 normal);
    EdgePoints clipEdge(glm::vec3& first, glm::vec3& second, glm::vec3 referenceEdge, f32 referenceOffset);

    f32 edgePreferenceDelta = 0.005f;
};
