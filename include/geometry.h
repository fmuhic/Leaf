#pragma once

#include <utility>
#include <vector>

#include "types.h"
#include "const.h"
#include "entity.h"
#include "body.h"
#include "leaf_math.h"

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

struct Geometry {
    Geometry(i32 maxEntityCount);

    void broadPhase(std::vector<Entity>& entities);
    void narrowPhase(std::vector<Entity>& entities);

    std::vector<Collision> collisions;

    private:

    bool aabbIntersect(AABB& a, AABB& b);
    void findPolygonPolygonContactPoints(RigidBody &a, RigidBody &b, Collision &c);
    Collision checkPlygonPolygon(RigidBody &a, RigidBody &b);

    std::vector<std::pair<i32, i32>> candidates; 
};
