#pragma once

#include <glm/glm.hpp>

#include "types.h"
#include "rigid_body/geometry.h"

struct Body {
    Body(
        BodyType type,
        glm::vec3 initPosition,
        glm::vec3 scale,
        bool immovable,
        f32 staticFriction,
        f32 dynamicFriction,
        f32 restitution
    );

    void step();

    glm::vec3 acceleration;
    glm::vec3 linearVelocity;
    glm::vec3 position;

    glm::vec3 angularVelocity;
    glm::vec3 orientation;

    f32 mass;
    f32 inverseMass;
    f32 inertia;
    f32 inverseInertia;
    f32 staticFriction;
    f32 dynamicFriction;
    f32 restitution;
};
