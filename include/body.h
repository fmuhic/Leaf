#pragma once

#include <glm/ext/matrix_transform.hpp>
#include "types.h"
#include "aabb.h"

#define MAX_VERTEX_COUNT 4

enum struct GeometryType: ui32 {
    BOX,
    CIRCLE
};

struct BodyConfig {
    glm::vec3 position = glm::vec3();
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    f32 rotation = 0.0f;
    f32 friction = 0.3f;
    f32 restitution = 0.5f;
    bool immovable = false;
};

struct RigidBody {
    RigidBody();
    RigidBody(BodyConfig config, GeometryType type);
    RigidBody(
        GeometryType type,
        glm::vec3 scale,
        bool immovable,
        glm::vec3 initPosition,
        f32 rotation,
        f32 staticFriction,
        f32 dynamicFriction,
        f32 restitution
    );

    void reset(glm::vec3 newPosition);
    void updateVelocity(f32 dt);
    void updatePosition(f32 dt);

    GeometryType type ;
    AABB aabb;
    glm::vec3 scale;
    glm::mat4 model;
    glm::vec3 vertices[MAX_VERTEX_COUNT];
    i32 vertexCount;

    glm::vec3 acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 linearVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 position;
    glm::vec3 oldPosition;

    f32 angularVelocity = 0.0f;
    f32 rotation = 0.0f;

    f32 mass;
    f32 inverseMass;
    f32 inertia;
    f32 inverseInertia;
    f32 staticFriction;
    f32 dynamicFriction;
    f32 restitution;

    i32 treeId = -1;
    bool isAlive = false;

    private:

    void transformToWorld();
    void updateAABB();
};
