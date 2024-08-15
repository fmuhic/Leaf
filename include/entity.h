#pragma once

#include <glm/ext/matrix_transform.hpp>
#include "types.h"

struct Entity {
    Entity() {}

    bool isAlive = false;
    i32 bodyId = -1;
    glm::vec3 color;
};
