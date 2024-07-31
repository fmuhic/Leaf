#pragma once

#include <vector>
#include <map>

#include "dynamic_tree.h"
#include "object_pool.h"
#include "list_pool.h"
#include "types.h"
#include "entity.h"
#include "body.h"
#include "collision.h"

#define COLLISION_COUNT ENTITY_COUNT * 4

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

struct Geometry {
    Geometry(i32 maxEntityCount, ListPool<glm::vec3>* vertices);

    void broadPhase(std::vector<Entity>& entities);
    void narrowPhase(std::vector<Entity>& entities);
    void narrowPhase(std::map<CollisionKey, Collision>& candidates, ObjectPool<RigidBody>& bodies);
    void reset();

    std::map<CollisionKey, Collision> collisions;
    DynamicTree* dynamicTree;

    private:

    bool aabbIntersect(AABB& a, AABB& b);
    Collision checkPlygonPolygon(RigidBody& a, RigidBody& b);
    void findPolygonPolygonContactPoints(RigidBody& a, RigidBody& b, Collision& c);
    void findContactPoints(RigidBody& a, RigidBody& b, Collision& c);
    Edge findContactEdge(glm::vec3* vertices, i32 count, glm::vec3 normal);
    EdgePoints clipEdge(glm::vec3& first, glm::vec3& second, glm::vec3 referenceEdge, f32 referenceOffset);

    std::vector<std::pair<i32, i32>> candidates; 
    ListPool<glm::vec3>* vertices;
    f32 edgePreferenceDelta = 0.005f;
};
