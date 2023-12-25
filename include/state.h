#pragma once

#include <glm/glm.hpp>

#include "types.h"
#include "body.h"

#define ENTITY_COUNT 100
#define MAX_COLISION_COUNT 10000
#define ENTITY_VERTEX_COUNT 4
#define COLOR_COUNT 14

enum class EntityType: ui32 {
    RECTANGLE = 0,
    CIRCLE = 1
};

struct KeyboardInput {
    bool leftPressed;
    bool rightPressed;
    bool upPressed;
    bool downPressed;
};

struct MouseInput {
    glm::vec3 position;
    bool leftClickPressed;
    bool leftClickReleased;
    bool leftClickClicked;

    bool rightClickPressed;
    bool rightClickReleased;
    bool rightClickClicked;
};

struct Scene {
    glm::mat4 camera;
    glm::vec3 cameraFollow;
    glm::mat4 projection;
    glm::mat4 inverseProjectionView;
};

struct Entity {
    RigidBody body;

    bool isAlive = false;
    bool isStatic = false;
    EntityType type = EntityType::RECTANGLE;
    glm::vec3 a = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 v = glm::vec3(0.0f, 0.0f, 0.0f);
    f32 omega = 0.0f;
    glm::vec3 p = glm::vec3(0.0f, 0.0f, 0.0f);
    AABB aabb;
    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 vertices[ENTITY_VERTEX_COUNT]{};
    bool transformed = false;
    glm::vec3 scale = glm::vec3(1.0f, 1.0, 1.0f);
    f32 angle = 0.0f;
    f32 r = 0.5f;
    glm::vec3 color;

    f32 mass;
    f32 inverseMass;
    f32 inertia;
    f32 inverseInertia;
    f32 restitution;
    f32 staticFriction = 0.65;
    f32 dynamicFriction = 0.45f;
};

struct CollisionManifold {
    Entity *a;
    Entity *b;
    bool colided = false;
    f32 depth = 0.0f;
    glm::vec3 normal;
    i32 contactPointsCount = 0;
    glm::vec3 cp1;
    glm::vec3 cp2;
};

struct Collisions {
    i32 count = 0;
    CollisionManifold manifolds[MAX_COLISION_COUNT];
};

struct Game {
    Entity entities[ENTITY_COUNT];  
    Entity player;
    Collisions collisions;
    glm::vec3 colors[COLOR_COUNT] {
        glm::vec3(0.80f, 0.38f, 0.33f),
        glm::vec3(0.93f, 0.44f, 0.39f),
        glm::vec3(0.69f, 0.48f, 0.77f),
        glm::vec3(0.65f, 0.41f, 0.74f),
        glm::vec3(0.33f, 0.60f, 0.78f),
        glm::vec3(0.36f, 0.68f, 0.89f),
        glm::vec3(0.28f, 0.79f, 0.69f),
        glm::vec3(0.27f, 0.70f, 0.62f),
        glm::vec3(0.32f, 0.75f, 0.50f),
        glm::vec3(0.35f, 0.84f, 0.55f),
        glm::vec3(0.96f, 0.82f, 0.25f),
        glm::vec3(0.96f, 0.69f, 0.25f),
        glm::vec3(0.92f, 0.60f, 0.31f),
        glm::vec3(0.86f, 0.46f, 0.20f)
    };
};
