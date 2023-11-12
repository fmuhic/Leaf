#include <cmath>
#include <glm/ext/matrix_transform.hpp>

#include "glm/ext/quaternion_geometric.hpp"
#include "helpers.h"
#include "physics.h"
#include "state.h"

void checkCollisions(Game *game) {
    for (auto &a: game->entities) {
        if (a.isAlive && a.type == EntityType::ENTITY_CIRCLE) {
            // a.model = glm::translate(a.model, glm::vec3(a.p.x, a.p.y, 0.0f));
            // a.model = glm::rotate(a.model, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
            // a.model = glm::scale(a.model, glm::vec3(a.scale, a.scale, 1.0f));

            for (auto &b: game->entities) {
                if (b.isAlive && b.type == EntityType::ENTITY_CIRCLE && &a != &b) {
                    if (pow(b.p.x - a.p.x, 2) + pow(b.p.y - a.p.y, 2) < pow(a.r * a.scale + b.r * b.scale, 2)) {
                        a.v -= 0.05f * glm::normalize(b.p - a.p);
                        b.v -= 0.05f * glm::normalize(a.p - b.p);
                    }
                }
            }
        }
    }
}
