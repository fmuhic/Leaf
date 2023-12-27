#pragma once

#include <utility>
#include <vector>

#include "types.h"
#include "state.h"
#include "body.h"

#define COLLISION_COUNT ENTITY_COUNT * 4

struct Collision {
    Entity *a;
    Entity *b;
    bool colided = false;
    f32 depth = 0.0f;
    glm::vec3 normal;
    i32 contactPointsCount = 0;
    glm::vec3 cp1;
    glm::vec3 cp2;
};

struct Geometry {
    Geometry();

    void broadPhase(std::vector<Entity>* entities);
    void narrowPhase(std::vector<Entity>* entities);
    void reset();

    private:

    bool aabbIntersect(AABB* a, AABB* b);
    std::pair<f32, f32> projectVerticesOnAxis(glm::vec3* vertices, i32 count, glm::vec3 axis);

    Collision checkPlygonPolygon(RigidBody *a, RigidBody *b);

    std::vector<std::pair<i32, i32>> candidates; 
    std::vector<Collision> collisions;
};
