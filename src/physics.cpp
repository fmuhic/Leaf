#include <cmath>
#include <assert.h>
#include <limits>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>

#include "glm/ext/quaternion_geometric.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
#include "helpers.h"
#include "mmath.h"
#include "physics.h"
#include "state.h"

f32 calculateMomentOfInertia(Entity *e) {
    if (e->type == EntityType::ENTITY_CIRCLE)
        return 0.5f * e->mass * e->r * e->r;
    else if (e->type == EntityType::ENTITY_QUAD)
        return (1.0f / 12.0f) * e->mass * (e->scale.x * e->scale.x + e->scale.y * e->scale.y);

    assert("Invalid entity type");
    return -1.0f;
}

void updateAABB(Entity *e) {
    if (e->type == EntityType::ENTITY_CIRCLE) {
        e->aabb.bottomLeft = e->p - e->r;
        e->aabb.topRight = e->p + e->r;
    }
    else if (e->type == EntityType::ENTITY_QUAD) {
        f32 xMin = e->vertices[0].x;
        f32 xMax = e->vertices[0].x;
        f32 yMin = e->vertices[0].y;
        f32 yMax = e->vertices[0].y;

        for (i32 i = 1; i < ENTITY_VERTEX_COUNT; i++) {
            glm::vec3 v = e->vertices[i];
            if (v.x < xMin) xMin = v.x;
            if (v.x > xMax) xMax = v.x;
            if (v.y < yMin) yMin = v.y;
            if (v.y > yMax) yMax = v.y;
        }

        e->aabb.bottomLeft = glm::vec3(xMin, yMin, 0.0f);
        e->aabb.topRight = glm::vec3(xMax, yMax, 0.0f);
    }
}

void transform(Entity *e) {
    if (e->transformed)
        return;

    e->model = glm::mat4(1.0f);
    e->model = glm::translate(e->model, e->p);
    e->model = glm::rotate(e->model, e->angle, glm::vec3(0.0f, 0.0f, 1.0f));
    e->model = glm::scale(e->model, glm::vec3(e->scale.x, e->scale.y, e->scale.z));

    e->vertices[0] = e->model * glm::vec4(0.5f, 0.5f, 0.0f, 1.0f);
    e->vertices[1] = e->model * glm::vec4(0.5f, -0.5f, 0.0f, 1.0f);
    e->vertices[2] = e->model * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
    e->vertices[3] = e->model * glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f);

    updateAABB(e);
}

glm::vec3 findClosestVertice(glm::vec3 p, glm::vec3 *vertices, i32 verticeCount) {
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

PointLineResult findClosestPointToLine(glm::vec3 *p, glm::vec3 *a, glm::vec3 *b) {
    glm::vec3 ab = *b - *a;
    glm::vec3 ap = *p - *a;
    f32 pr = glm::dot(ap, ab);
    f32 abLengthSq = lengthSq(&ab);
    f32 abSegSq = pr / abLengthSq;

    PointLineResult r{};
    if (abSegSq <= 0.0f)
        r.cp = *a;
    else if (abSegSq >= 1.0f)
        r.cp = *b;
    else
        r.cp = *a + ab * abSegSq;

    r.distSq = distanceSq(p, &r.cp);

    return r;
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

CollisionManifold checkCircleCircle(Entity *a, Entity *b) {
    CollisionManifold r{};
    glm::vec3 d = b->p - a->p;
    r.depth = a->r + b->r - glm::length(d);
    if (r.depth > 0.0f) {
        r.colided = true;
        r.normal = glm::normalize(d);
    }
    return r;
}

CollisionManifold checkPlygonCircle(Entity *p, Entity *c) {
    CollisionManifold r{};
    r.depth = std::numeric_limits<f32>::max();

    for (i32 i = 0; i < ENTITY_VERTEX_COUNT; i++) {
        glm::vec3 edge = p->vertices[(i + 1) % ENTITY_VERTEX_COUNT] - p->vertices[i];
        glm::vec3 axis = glm::normalize(glm::vec3(-edge.y, edge.x, 0.0f));
        Projection pp = projectVertices(p->vertices, ENTITY_VERTEX_COUNT, axis);
        Projection cp = projectCircle(&c->p, c->r, &axis);

        if (pp.min > cp.max || cp.min > pp.max) {
            r.colided = false;
            return r;
        }

        f32 depth = min(pp.max - cp.min, cp.max - pp.min);
        if (depth < r.depth) {
            r.depth = depth;
            r.normal = axis;
        }
    }
    
    glm::vec3 axis = glm::normalize(findClosestVertice(c->p, p->vertices, ENTITY_VERTEX_COUNT) - c->p);
    Projection pp = projectVertices(p->vertices, ENTITY_VERTEX_COUNT, axis);
    Projection cp = projectCircle(&c->p, c->r, &axis);

    if (pp.min > cp.max || cp.min > pp.max) {
        r.colided = false;
        return r;
    }

    f32 depth = min(pp.max - cp.min, cp.max - pp.min);
    if (depth < r.depth) {
        r.depth = depth;
        r.normal = axis;
    }

    r.colided = true;
    r.depth = r.depth / glm::length(r.normal);
    r.normal = glm::normalize(r.normal);

    glm::vec3 ab = c->p - p->p;
    if (glm::dot(ab, r.normal) < 0.0f)
        r.normal = -r.normal;

    return r;
}

// Fixed vertices for now, until we get proper memory allocator
CollisionManifold checkPlygonPolygon(Entity *a, Entity *b) {
    CollisionManifold r{};
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
            r.normal = axis;
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
            r.normal = axis;
        }
    }

    r.colided = true;

    glm::vec3 ab = b->p - a->p;
    if (glm::dot(ab, r.normal) < 0.0f)
        r.normal = -r.normal;

    return r;
}

void separateEntities(Entity *a, Entity *b, CollisionManifold *m) {
    if (a->isStatic)
        b->p += m->depth * m->normal;
    else if (b->isStatic)
        a->p -= m->depth * m->normal;
    else {
        a->p -= 0.5f * m->depth * m->normal;
        b->p += 0.5f * m->depth * m->normal;
    }
}

void findCircleCircleContactPoints(Entity *a, Entity *b, CollisionManifold *m) {
    glm::vec3 ab = b->p - a->p;
    glm::vec3 n = normalize(ab);
    m->contactPointsCount = 1;
    m->cp1 = a->p + n * a->r;
}

void findPolygonCircleContactPoints(Entity *p, Entity *c, CollisionManifold *m) {
    m->contactPointsCount = 1;
    f32 minSqDistance = std::numeric_limits<f32>::max();
    for (i32 i = 0; i < ENTITY_VERTEX_COUNT; i++) {
        glm::vec3 a = p->vertices[i];
        glm::vec3 b = p->vertices[(i + 1) % ENTITY_VERTEX_COUNT];
        PointLineResult res = findClosestPointToLine(&c->p, &a, &b);
        if (res.distSq < minSqDistance) {
            minSqDistance = res.distSq;
            m->cp1 = res.cp;
        }
    }
}

bool closeTo(f32 a, f32 b, f32 delta) {
    return fabs(a - b) < delta;
}

bool closeTo(glm::vec3 *a, glm::vec3 *b, f32 delta) {
    return closeTo(a->x, b->x, delta) && closeTo(a->y, b->y, delta);
}

void findPolygonPolygonContactPoints(Entity *a, Entity *b, CollisionManifold *m) {
    f32 delta = 0.0005f;
    f32 minSqDistance = std::numeric_limits<f32>::max();
    for (i32 i = 0; i < ENTITY_VERTEX_COUNT; i++) {
        glm::vec3 v = a->vertices[i];
        for (i32 j = 0; j < ENTITY_VERTEX_COUNT; j++) {
            PointLineResult res = findClosestPointToLine(&v, &b->vertices[j], &b->vertices[(j + 1) % ENTITY_VERTEX_COUNT]);
            if (closeTo(res.distSq, minSqDistance, delta)) {
                if (!closeTo(&res.cp, &m->cp1, delta)) {
                    m->cp2 = res.cp;
                    m->contactPointsCount = 2;
                }
            }
            else if (res.distSq < minSqDistance) {
                minSqDistance = res.distSq;
                m->cp1 = res.cp;
                m->contactPointsCount = 1;
            }
        }
    }
    for (i32 i = 0; i < ENTITY_VERTEX_COUNT; i++) {
        glm::vec3 v = b->vertices[i];
        for (i32 j = 0; j < ENTITY_VERTEX_COUNT; j++) {
            PointLineResult res = findClosestPointToLine(&v, &a->vertices[j], &a->vertices[(j + 1) % ENTITY_VERTEX_COUNT]);
            if (closeTo(res.distSq, minSqDistance, delta)) {
                if (!closeTo(&res.cp, &m->cp1, delta)) {
                    m->cp2 = res.cp;
                    m->contactPointsCount = 2;
                }
            }
            else if (res.distSq < minSqDistance) {
                minSqDistance = res.distSq;
                m->cp1 = res.cp;
                m->contactPointsCount = 1;
            }
        }
    }
}

void findContactPoints(Entity *a, Entity *b, CollisionManifold *m) {
    if (a->type == EntityType::ENTITY_CIRCLE && b->type == EntityType::ENTITY_CIRCLE)
        findCircleCircleContactPoints(a, b, m);
    else if (a->type == EntityType::ENTITY_QUAD && b->type == EntityType::ENTITY_CIRCLE)
        findPolygonCircleContactPoints(a, b, m);
    else if (a->type == EntityType::ENTITY_CIRCLE && b->type == EntityType::ENTITY_QUAD)
        findPolygonCircleContactPoints(b, a, m);
    else if (a->type == EntityType::ENTITY_QUAD && b->type == EntityType::ENTITY_QUAD) 
        findPolygonPolygonContactPoints(b, a, m);
}

void resolve(CollisionManifold *m) {
    if (!m->colided)
        return;

    Entity *a = m->a;
    Entity *b = m->b;

    separateEntities(a, b, m);
    findContactPoints(a, b, m);

    f32 e = min(a->restitution, b->restitution);

    glm::vec3 cp[]{ m->cp1, m->cp2 };
    glm::vec3 impulses[2] = {};
    f32 impulseLen[2] = {};

    for (i32 i = 0; i < m->contactPointsCount; i++) {
        glm::vec3 pa = cp[i] - a->p;
        glm::vec3 pb = cp[i] - b->p;

        glm::vec3 pap = glm::vec3(-pa.y, pa.x, 0.0f);
        glm::vec3 pbp = glm::vec3(-pb.y, pb.x, 0.0f);

        glm::vec3 omegaA = a->omega * pap;
        glm::vec3 omegaB = b->omega * pbp;

        glm::vec3 va = a->v + omegaA; 
        glm::vec3 vb = b->v + omegaB; 

        glm::vec3 vRelative = vb - va;

        f32 contactVelocityLen = glm::dot(vRelative, m->normal);

        if (contactVelocityLen > 0.0f)
            continue;

        f32 j = -(1 + e) * contactVelocityLen /
            (a->inverseMass + b->inverseMass + pow(glm::dot(pap, m->normal), 2) * a->inverseInertia + pow(glm::dot(pbp, m->normal), 2) * b->inverseInertia);

        j /= (f32) m->contactPointsCount;

        impulseLen[i] = j;
        impulses[i] = j * m->normal;
    }

    for (i32 i = 0; i < m->contactPointsCount; i++) {
        glm::vec3 pa = cp[i] - a->p;
        glm::vec3 pb = cp[i] - b->p;

        a->v -= impulses[i] * a->inverseMass;
        a->omega -= cross(&pa, &impulses[i]) * a->inverseInertia;
        b->v += impulses[i] * b->inverseMass;
        b->omega += cross(&pb, &impulses[i]) * b->inverseInertia;
    }

    glm::vec3 frictionImpulses[2]{};
    f32 staticFriction = (a->staticFriction + b->staticFriction) * 0.5f;
    f32 dynamicFriction = (a->dynamicFriction + b->dynamicFriction) * 0.5f;
    // Friction
    for (i32 i = 0; i < m->contactPointsCount; i++) {
        glm::vec3 pa = cp[i] - a->p;
        glm::vec3 pb = cp[i] - b->p;

        glm::vec3 pap = glm::vec3(-pa.y, pa.x, 0.0f);
        glm::vec3 pbp = glm::vec3(-pb.y, pb.x, 0.0f);

        glm::vec3 omegaA = a->omega * pap;
        glm::vec3 omegaB = b->omega * pbp;

        glm::vec3 va = a->v + omegaA; 
        glm::vec3 vb = b->v + omegaB; 

        glm::vec3 vRelative = vb - va;

        glm::vec3 tangent = vRelative - glm::dot(vRelative, m->normal) * m->normal;

        glm::vec3 zero = glm::vec3(0.0f, 0.0f, 0.0f);
        f32 delta = 0.005;
        if (closeTo(&tangent, &zero, delta))
            continue;
        else
            tangent = glm::normalize(tangent);

        f32 jf = -glm::dot(vRelative, tangent) /
            (a->inverseMass + b->inverseMass + pow(glm::dot(pap, tangent), 2) * a->inverseInertia + pow(glm::dot(pbp, tangent), 2) * b->inverseInertia);

        jf /= (f32) m->contactPointsCount;

        if (fabs(jf) < impulseLen[i] * staticFriction)
            frictionImpulses[i] = jf * tangent;
        else
            frictionImpulses[i] = -impulseLen[i] * tangent * dynamicFriction;
    }

    for (i32 i = 0; i < m->contactPointsCount; i++) {
        glm::vec3 pa = cp[i] - a->p;
        glm::vec3 pb = cp[i] - b->p;

        a->v -= frictionImpulses[i] * a->inverseMass;
        a->omega -= cross(&pa, &frictionImpulses[i]) * a->inverseInertia;
        b->v += frictionImpulses[i] * b->inverseMass;
        b->omega += cross(&pb, &frictionImpulses[i]) * b->inverseInertia;
    }
}

bool AABBcolliding(AABB *a, AABB *b) {
    if (a->topRight.x < b->bottomLeft.x || a->bottomLeft.x > b->topRight.x ||
        a->topRight.y < b->bottomLeft.y || a->bottomLeft.y > b->topRight.y)
            return false;
    return true;
}

void checkCollisions(Game *game) {
    game->collisions.count = 0;
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

            if (!AABBcolliding(&a.aabb, &b.aabb))
                continue;

            CollisionManifold cm{};
            if (a.type == EntityType::ENTITY_CIRCLE && b.type == EntityType::ENTITY_CIRCLE)
                cm = checkCircleCircle(&a, &b);
            if (a.type == EntityType::ENTITY_QUAD && b.type == EntityType::ENTITY_CIRCLE)
                cm = checkPlygonCircle(&a, &b);
            if (a.type == EntityType::ENTITY_CIRCLE && b.type == EntityType::ENTITY_QUAD) {
                cm = checkPlygonCircle(&b, &a);
                cm.normal *= -1;
            }
            if (a.type == EntityType::ENTITY_QUAD && b.type == EntityType::ENTITY_QUAD)
                cm = checkPlygonPolygon(&a, &b);

            cm.a = &game->entities[i];
            cm.b = &game->entities[j];
            game->collisions.manifolds[game->collisions.count++] = cm;
        }
    }
}

void resolveCollisions(Game *game) {
    for (i32 i = 0; i < game->collisions.count; i++) {
        CollisionManifold *m = &game->collisions.manifolds[i];
        resolve(m);
    }
}
