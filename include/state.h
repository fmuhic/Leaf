#pragma once

#include <glm/glm.hpp>

#include "types.h"
#include "body.h"

#define ENTITY_COUNT 100
#define MAX_COLISION_COUNT 10000
#define ENTITY_VERTEX_COUNT 4
#define COLOR_COUNT 14

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
