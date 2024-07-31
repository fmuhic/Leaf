#pragma once

#include <glm/ext/matrix_transform.hpp>

#include "list_pool.h"
#include "types.h"

#define MAX_VERTEX_COUNT 4

struct AABB {
    AABB() {};
    AABB(glm::vec3 bottom, glm::vec3 top): bottomLeft(bottom), topRight(top) {}

    AABB fatten(const f32 amount) const;
    AABB merge(const AABB& other) const;
    bool contains(AABB& other) const;
    bool overlaps(AABB& other) const;
    f32 perimiter() const;

    glm::vec3 bottomLeft;
    glm::vec3 topRight;
};

enum struct GeometryType: ui32 {
    BOX = 0,
    CIRCLE = 1
};

enum struct BodyType: ui32 {
    DYNAMIC = 0,
    KINEMATIC = 1,
    STATIC = 2
};

struct UserData {
    i32 entityId = -1;
};

struct BodyConfig {
    glm::vec3 position = glm::vec3();
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    f32 orientation = 0.0f;
    f32 friction = 0.5f;
    f32 restitution = 0.5f;
    bool immovable = false;
};

struct RigidBody {
    RigidBody();
    RigidBody(
        GeometryType type,
        glm::vec3 scale,
        bool immovable,
        glm::vec3 initPosition,
        f32 orientation,
        f32 staticFriction,
        f32 dynamicFriction,
        f32 restitution
    );
    RigidBody(BodyConfig config, GeometryType type, UserData data = UserData());

    void reset(glm::vec3 newPosition);
    void updateVelocity(f32 dt);
    void updatePosition(f32 dt);
    void update(ListPool<glm::vec3>& vertices);

    GeometryType type ;
    AABB aabb;
    glm::vec3 scale;
    glm::mat4 model;
    glm::vec3 vertices[MAX_VERTEX_COUNT];
    i32 vertexCount;
    i32 verticesId;

    glm::vec3 acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 linearVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 position;
    glm::vec3 prevPosition;

    f32 angularVelocity = 0.0f;
    f32 orientation = 0.0f;

    f32 mass;
    f32 inverseMass;
    f32 inertia;
    f32 inverseInertia;
    f32 staticFriction;
    f32 dynamicFriction;
    f32 restitution;

    i32 treeId;
    i32 collisionCandidates = -1;
    UserData data;

    private:

    // Duplicates
    void transformToWorld();
    void updateAABB();

    void transformToWorld(ListPool<glm::vec3>& vertices);
    void updateAABB(ListPool<glm::vec3>& vertices);
};
