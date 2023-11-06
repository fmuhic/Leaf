#pragma once

#include <cstdint>
#include <unordered_map>
#include <string>
#include <glm/mat4x4.hpp>

#include "types.h"

#define ENTITY_COUNT 1000

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

struct Camera {
    glm::mat4 focus;
};

struct Entity {
    bool isAlive = false;
    glm::vec3 p = glm::vec3(0.0f, 0.0f, 0.0f);
    f32 scale = 50.0f;
};

struct Game {
    Entity entities[ENTITY_COUNT];  
};
