#pragma once

#include <glm/ext/matrix_transform.hpp>
#include "types.h"

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
