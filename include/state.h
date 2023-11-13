#pragma once

#include <glm/glm.hpp>

#include "types.h"

#define ENTITY_COUNT 100
#define COLOR_COUNT 14

enum class EntityType: ui32 {
    ENTITY_QUAD = 0,
    ENTITY_CIRCLE = 1
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
};

struct Scene {
    glm::mat4 camera;
    glm::vec3 cameraFollow;
    glm::mat4 projection;
    glm::mat4 inverseProjectionView;
};

struct Entity {
    bool isAlive = false;
    EntityType type = EntityType::ENTITY_QUAD;
    glm::vec3 a = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 v = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 p = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 vertices[4]{};
    bool transformed = false;
    f32 scale = 1.0f;
    f32 r = 0.5f;
    glm::vec3 color;
};

struct Game {
    Entity entities[ENTITY_COUNT];  
    Entity player;
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
