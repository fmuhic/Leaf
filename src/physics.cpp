#include <cmath>
#include <glm/ext/matrix_transform.hpp>

#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
#include "helpers.h"
#include "physics.h"
#include "state.h"

void transform(Entity *e) {
    if (!e->transformed) {
        e->model = glm::mat4(1.0f);
        e->model = glm::translate(e->model, e->p);
        e->model = glm::rotate(e->model, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
        e->model = glm::scale(e->model, glm::vec3(e->scale, e->scale, 1.0f));

        e->vertices[0] = e->model * glm::vec4(0.5f, 0.5f, 0.0f, 1.0f);
        e->vertices[1] = e->model * glm::vec4(0.5f, -0.5f, 0.0f, 1.0f);
        e->vertices[2] = e->model * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
        e->vertices[3] = e->model * glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f);

        e->transformed = true;
    }
}

CollisionResult checkCircleCircle(Entity *a, Entity *b) {
    CollisionResult r{};
    glm::vec3 d = b->p - a->p;
    r.depth = a->r * a->scale + b->r * b->scale - glm::length(d);
    if (r.depth > 0.0f) {
        r.colided = true;
        r.direction = glm::normalize(d);
    }
    return r;
}

// Separating Axis Theorem (SAT)
CollisionResult checkPlygonPolygon(Entity *a, Entity *b) {
    CollisionResult r{};
    return r;
}

void checkCollisions(Game *game) {
    for (i32 i = 0; i < ENTITY_COUNT - 1; ++i) {
        Entity &a = game->entities[i];
        if (!a.isAlive)
            continue;
        transform(&a);

        for (i32 j = i + 1; j < ENTITY_COUNT; ++j) {
            Entity &b = game->entities[j];
            if (!b.isAlive)
                continue;

            transform(&b);
            CollisionResult r{};
            if (a.type == EntityType::ENTITY_CIRCLE && b.type == EntityType::ENTITY_CIRCLE)
                r = checkCircleCircle(&a, &b);
            if (a.type == EntityType::ENTITY_QUAD && b.type == EntityType::ENTITY_QUAD)
                r = checkPlygonPolygon(&a, &b);

            if (r.colided) {
                a.p -= 0.5f * r.depth * r.direction;
                b.p += 0.5f * r.depth * r.direction;
            }
        }
    }
}


