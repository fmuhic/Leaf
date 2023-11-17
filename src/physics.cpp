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

glm::vec3 findClosestPoint(glm::vec3 p, glm::vec3 *vertices, i32 verticeCount) {
    assert(verticeCount > 0);
    f32 dmin = glm::length(vertices[0] - p);
    glm::vec3 res = vertices[0];
    for (i32 i = 1; i < verticeCount; i++) {
        f32 d = glm::length(vertices[i] - p);
        if (d < dmin) {
            dmin = d;
            res = vertices[i];
        }
    }

    return res;
}

Projection projectVertices(glm::vec3 *vertices, i32 count, glm::vec3 axis) {
    f32 min = std::numeric_limits<f32>::max();
    f32 max = std::numeric_limits<float>::min();
    for (i32 i = 0; i < count; i++) {
        f32 p = glm::dot(vertices[i], axis);
        if (p < min) min = p;
        if (p > max) max = p;
    }

    return Projection { min, max };
}

Projection projectCircle(glm::vec3 *p, f32 r, glm::vec3 *axis) {
    f32 center = glm::dot(*p, *axis);
    return Projection { center - r, center + r };
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

CollisionResult checkPlygonCircle(Entity *p, Entity *c) {
    CollisionResult r{};
    r.depth = std::numeric_limits<f32>::max();

    for (i32 i = 0; i < ENTITY_VERTEX_COUNT; i++) {
        glm::vec3 edge = p->vertices[(i + 1) % ENTITY_VERTEX_COUNT] - p->vertices[i];
        glm::vec3 axis = glm::normalize(glm::vec3(-edge.y, edge.x, 0.0f));
        Projection pp = projectVertices(p->vertices, ENTITY_VERTEX_COUNT, axis);
        Projection cp = projectCircle(&c->p, c->r * c->scale, &axis);

        if (pp.min > cp.max || cp.min > pp.max) {
            r.colided = false;
            return r;
        }

        f32 depth = min(pp.max - cp.min, cp.max - pp.min);
        if (depth < r.depth) {
            r.depth = depth;
            r.direction = axis;
        }
    }
    
    glm::vec3 axis = glm::normalize(findClosestPoint(c->p, p->vertices, ENTITY_VERTEX_COUNT) - c->p);
    Projection pp = projectVertices(p->vertices, ENTITY_VERTEX_COUNT, axis);
    Projection cp = projectCircle(&c->p, c->r * c->scale, &axis);

    if (pp.min > cp.max || cp.min > pp.max) {
        r.colided = false;
        return r;
    }

    f32 depth = min(pp.max - cp.min, cp.max - pp.min);
    if (depth < r.depth) {
        r.depth = depth;
        r.direction = axis;
    }

    r.colided = true;
    r.depth = r.depth / glm::length(r.direction);
    r.direction = glm::normalize(r.direction);

    glm::vec3 ab = c->p - p->p;
    if (glm::dot(ab, r.direction) < 0.0f)
        r.direction = -r.direction;

    return r;
}

// Fixed vertices for now, until we get proper memory allocator
CollisionResult checkPlygonPolygon(Entity *a, Entity *b) {
    CollisionResult r{};
    r.depth = std::numeric_limits<f32>::max();

    for (i32 i = 0; i < ENTITY_VERTEX_COUNT; i++) {
        glm::vec3 edge = a->vertices[(i + 1) % ENTITY_VERTEX_COUNT] - a->vertices[i];
        glm::vec3 axis = glm::normalize(glm::vec3(-edge.y, edge.x, 0.0f));
        Projection ap = projectVertices(a->vertices, ENTITY_VERTEX_COUNT, axis);
        Projection bp = projectVertices(b->vertices, ENTITY_VERTEX_COUNT, axis);

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
        glm::vec3 axis = glm::normalize(glm::vec3(-edge.y, edge.x, 0.0f));
        Projection ap = projectVertices(a->vertices, ENTITY_VERTEX_COUNT, axis);
        Projection bp = projectVertices(b->vertices, ENTITY_VERTEX_COUNT, axis);

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

    glm::vec3 ab = b->p - a->p;
    if (glm::dot(ab, r.direction) < 0.0f)
        r.direction = -r.direction;

    return r;
}

void resolveCollision(CollisionResult *r, Entity *a, Entity *b) {
    if (!r->colided)
        return;

    glm::vec3 vba = b->v - a->v;
    // if (glm::dot(vba, r->direction) > 0.0f)
    //     return;

    f32 e = min(a->restitution, b->restitution);
    f32 j = -(1 + e) * glm::dot(vba, r->direction) / (a->inverseMass + b->inverseMass);

    a->v -= j * a->inverseMass * r->direction;
    b->v += j * b->inverseMass * r->direction;

    if (a->isStatic)
        b->p += r->depth * r->direction;
    else if (b->isStatic)
        a->p -= r->depth * r->direction;
    else {
        a->p -= 0.5f * r->depth * r->direction;
        b->p += 0.5f * r->depth * r->direction;
    }

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

            if (a.isStatic && b.isStatic)
                continue;

            CollisionResult r{};
            if (a.type == EntityType::ENTITY_CIRCLE && b.type == EntityType::ENTITY_CIRCLE)
                r = checkCircleCircle(&a, &b);
            if (a.type == EntityType::ENTITY_QUAD && b.type == EntityType::ENTITY_CIRCLE)
                r = checkPlygonCircle(&a, &b);
            if (a.type == EntityType::ENTITY_CIRCLE && b.type == EntityType::ENTITY_QUAD) {
                r = checkPlygonCircle(&b, &a);
                r.direction *= -1;
            }
            if (a.type == EntityType::ENTITY_QUAD && b.type == EntityType::ENTITY_QUAD)
                r = checkPlygonPolygon(&a, &b);

            resolveCollision(&r, &a, &b);
        }
    }
}
