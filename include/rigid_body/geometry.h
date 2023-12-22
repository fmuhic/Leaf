#pragma once

#include <glm/glm.hpp>

#include "types.h"

#define MAX_VERTEX_COUNT 4

struct AABB {
    glm::vec3 bottomLeft;
    glm::vec3 topRight;
};

enum class BodyType: ui32 {
    RECTANGLE = 0,
    CIRCLE = 1
};

struct Geometry {
    Geometry(BodyType type): type(type) {}
    
    void transformToWorldSpace();
    void reset();

    BodyType type;
    bool transformed = false;
    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 vertices[MAX_VERTEX_COUNT] {};
};
