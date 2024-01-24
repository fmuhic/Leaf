#pragma once

#include <utility>
#include <vector>

#include "types.h"
#include "const.h"
#include "entity.h"
#include "body.h"
#include "leaf_math.h"

#define MAX_CONTACT_POINTS 2
#define COLLISION_COUNT ENTITY_COUNT * 4

struct EdgePoints {
    i32 count = 0;
    glm::vec3 points[2];
};

struct Contact {
    glm::vec3 point;
    f32 depth;

    f32 inverseNormalMass;
    f32 inverseTangentMass;
    f32 positionCorrection;

    f32 normalImpulseLen;
    f32 friction;
};

struct Collision {
    std::pair<i32, i32> entities;
    bool colided = false;
    glm::vec3 normal;
    glm::vec3 tangent;
    i32 contactCount = 0;
    Contact contacts[MAX_CONTACT_POINTS];
};

struct Edge {
    glm::vec3 max;
    glm::vec3 first;
    glm::vec3 second;
};

struct Geometry {
    Geometry(i32 maxEntityCount);

    void broadPhase(std::vector<Entity>& entities);
    void narrowPhase(std::vector<Entity>& entities);

    std::vector<Collision> collisions;
    std::vector<Collision> clippingCollisions;

    private:

    bool aabbIntersect(AABB& a, AABB& b);
    Collision checkPlygonPolygon(RigidBody& a, RigidBody& b);
    void findPolygonPolygonContactPoints(RigidBody& a, RigidBody& b, Collision& c);
    void findContactPoints(RigidBody& a, RigidBody& b, Collision& c);
    Edge findContactEdge(glm::vec3* vertices, i32 count, glm::vec3 normal);
    EdgePoints clipEdge(glm::vec3& first, glm::vec3& second, glm::vec3 referenceEdge, f32 referenceOffset);

    std::vector<std::pair<i32, i32>> candidates; 
};
