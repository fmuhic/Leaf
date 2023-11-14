#include <cmath>
#include <limits>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>

#include "glm/ext/quaternion_geometric.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
#include "helpers.h"
#include "physics.h"
#include "state.h"

void transform(Entity *e) {
    if (!e->transformed) {
        e->model = glm::mat4(1.0f);
        e->model = glm::translate(e->model, e->p);
        e->model = glm::rotate(e->model, e->angle, glm::vec3(0.0f, 0.0f, 1.0f));
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

Projection project(glm::vec3 *vertices, i32 count, glm::vec3 axis) {
    f32 min = std::numeric_limits<f32>::max();
    f32 max = std::numeric_limits<float>::min();
    for (i32 i = 0; i < count; i++) {
        f32 p = glm::dot(vertices[i], axis);
        if (p < min) min = p;
        if (p > max) max = p;
    }

    return Projection { min, max };
}


// Fixed vertices for now, until we get proper memory allocator
CollisionResult checkPlygonPolygon(Entity *a, Entity *b) {
    CollisionResult r{};
    r.depth = std::numeric_limits<f32>::max();

    for (i32 i = 0; i < ENTITY_VERTEX_COUNT; i++) {
        glm::vec3 edge = a->vertices[(i + 1) % ENTITY_VERTEX_COUNT] - a->vertices[i];
        glm::vec3 axis = glm::vec3(-edge.y, edge.x, 0.0f);
        Projection ap = project(a->vertices, ENTITY_VERTEX_COUNT, axis);
        Projection bp = project(b->vertices, ENTITY_VERTEX_COUNT, axis);

        if (ap.min > bp.max || bp.min > ap.max) {
            r.colided = false;
            return r;
        }

        f32 depth = min(ap.max - bp.min, bp.max - ap.min);
        if (depth < r.depth) {
            r.depth = depth;
            r.direction = axis;
        }
    }

    for (i32 i = 0; i < ENTITY_VERTEX_COUNT; i++) {
        glm::vec3 edge = b->vertices[(i + 1) % ENTITY_VERTEX_COUNT] - b->vertices[i];
        glm::vec3 axis = glm::vec3(-edge.y, edge.x, 0.0f);
        Projection ap = project(a->vertices, ENTITY_VERTEX_COUNT, axis);
        Projection bp = project(b->vertices, ENTITY_VERTEX_COUNT, axis);

        if (ap.min > bp.max || bp.min > ap.max) {
            r.colided = false;
            return r;
        }

        f32 depth = min(ap.max - bp.min, bp.max - ap.min);
        if (depth < r.depth) {
            r.depth = depth;
            r.direction = axis;
        }
    }

    r.colided = true;
    r.depth = r.depth / glm::length(r.direction);
    r.direction = glm::normalize(r.direction);

    glm::vec3 ab = b->p - a->p;

    if (glm::dot(ab, r.direction) < 0.0f) {
        r.direction = -r.direction;
    }

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


