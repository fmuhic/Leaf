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

struct Contact {
    i32 count = 0;
    glm::vec3 points[2];
};

struct Collision {
    std::pair<i32, i32> entities;
    bool colided = false;
    f32 depth = 0.0f;
    glm::vec3 normal;
    i32 contactCount = 0;
    glm::vec3 points[MAX_CONTACT_POINTS];
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
    Contact clipEdge(glm::vec3& first, glm::vec3& second, glm::vec3 referenceEdge, f32 referenceOffset);

    std::vector<std::pair<i32, i32>> candidates; 
};
