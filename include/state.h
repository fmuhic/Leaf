#pragma once

#include <cstdint>
#include <unordered_map>
#include <string>
#include <glm/mat4x4.hpp>

#include "types.h"

struct UserInput {
    bool leftPressed;
    bool rightPressed;
    bool upPressed;
    bool downPressed;
};

struct Camera {
    glm::mat4 focus;
};
