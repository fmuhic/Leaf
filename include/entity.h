#pragma once

#include "types.h"
#include "body.h"
#include "const.h"
#include "helpers.h"

struct Entity {
    void despawnIfOutOfBounds() {
        if (body.position.x < -50.0f || body.position.y < -50.0f)
            destroy();
    }

    void destroy() {
        isAlive = false;
    }

    void activate(glm::vec3 position) {
        isAlive = true;
        body.reset(position);
    }

    bool isAlive = false;
    RigidBody body;
    glm::vec3 color = COLORS[pickRand(0, COLORS.size() - 1)];
};
