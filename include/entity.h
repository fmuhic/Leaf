#pragma once

#include "body.h"
#include "const.h"
#include "helpers.h"
#include <cassert>

struct Entity {
    bool despawnIfOutOfBounds() {
        if (body.position.x < -50.0f || body.position.y < -50.0f) {
            destroy();
            return true;
        }
        return false;
    }

    void destroy() {
        isAlive = false;
    }

    void activate(glm::vec3 position) {
        assert(treeId == -1);
        isAlive = true;
        body.reset(position);
    }

    bool isAlive = false;
    RigidBody body;
    i32 treeId = -1;
    glm::vec3 color = COLORS[pickRand(0, COLORS.size() - 1)];
};
