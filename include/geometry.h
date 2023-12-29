#pragma once

#include <utility>
#include <vector>

#include "types.h"
#include "entity.h"
#include "body.h"
#include "mmath.h"

#define COLLISION_COUNT ENTITY_COUNT * 4

struct Collision {
    std::pair<i32, i32> entities;
    bool colided = false;
    f32 depth = 0.0f;
    glm::vec3 normal;
    i32 contactPointsCount = 0;
    glm::vec3 cp1;
    glm::vec3 cp2;
};

struct PointLineResult {
    glm::vec3 cp;
    f32 distSq;
};

struct Geometry {
    Geometry(i32 maxEntityCount);

    void broadPhase(std::vector<Entity>* entities);
    void narrowPhase(std::vector<Entity>* entities);
    void reset();

    std::vector<Collision> collisions;

    private:

    std::pair<f32, f32> projectVerticesOnAxis(glm::vec3* vertices, i32 count, glm::vec3 axis);
    PointLineResult findClosestPointToLine(glm::vec3 &p, glm::vec3 &a, glm::vec3 &b); 

    bool aabbIntersect(AABB* a, AABB* b);
    void findPolygonPolygonContactPoints(RigidBody &a, RigidBody &b, Collision &c);
    Collision checkPlygonPolygon(RigidBody *a, RigidBody *b);

    std::vector<std::pair<i32, i32>> candidates; 
};
