#pragma once

#include <glm/ext/matrix_transform.hpp>

#include "types.h"

#define MAX_VERTEX_COUNT 4

struct AABB {
    glm::vec3 bottomLeft;
    glm::vec3 topRight;
};

enum struct BodyType: ui32 {
    RECTANGLE,
    CIRCLE
};

struct RigidBody {
    RigidBody();
    RigidBody(
        BodyType type,
        glm::vec3 scale,
        glm::vec3 initPosition,
        bool immovable,
        f32 staticFriction,
        f32 dynamicFriction,
        f32 restitution
    );

    void reset();
    void step(f32 dt);

    BodyType type ;
    AABB aabb;
    glm::vec3 scale;
    glm::mat4 model;
    glm::vec3 vertices[MAX_VERTEX_COUNT];
    i32 vertexCount;

    glm::vec3 acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 linearVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 position;

    f32 angularVelocity = 0.0f;
    f32 orientation = 0.0f;

    f32 mass;
    f32 inverseMass;
    f32 inertia;
    f32 inverseInertia;
    f32 staticFriction;
    f32 dynamicFriction;
    f32 restitution;

    private:

    void transformToWorld();
    void updateAABB();
};
