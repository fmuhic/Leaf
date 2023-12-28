#include "geometry.h"

using std::pair;
using std::vector;

Geometry::Geometry() {
    candidates.reserve(COLLISION_COUNT);
    collisions.reserve(COLLISION_COUNT);
}

void Geometry::broadPhase(vector<Entity>* entities) {
    candidates.clear();

    // Switch to spatial partitioning
    for (ui32 i = 0; i < entities->size() - 1; ++i) {
        Entity &a = entities->at(i);
        if (!a.isAlive)
            continue;

        for (ui32 j = i + 1; j < entities->size(); ++j) {
            Entity &b = entities->at(j);
            if (!b.isAlive)
                continue;

            if (a.body.inverseMass == 0.0f && b.body.inverseMass == 0.0f)
                continue;

            if (!aabbIntersect(&a.aabb, &b.aabb))
                continue;

            candidates.push_back(std::pair(i, j));
        }
    }
}

void Geometry::narrowPhase(std::vector<Entity>* entities) {
    collisions.clear();

    for (auto &candidate: candidates) {
        Entity &a = entities->at(candidate.first);
        Entity &b = entities->at(candidate.second);

        Collision c;
        if (a.type == EntityType::RECTANGLE && b.type == EntityType::RECTANGLE)
            c = checkPlygonPolygon(&a.body, &b.body);
        else
            assert(false && "Invalid object types");
        if (c.colided) { 

            // Remove this after clipping
            if (a.body.inverseMass == 0.0f) 
                b.body.position += c.depth * c.normal;
            else if (b.body.inverseMass == 0.0f)
                a.body.position -= c.depth * c.normal;
            else {
                a.body.position -= 0.5f * c.depth * c.normal;
                b.body.position += 0.5f * c.depth * c.normal;
            }
            findPolygonPolygonContactPoints(a.body, b.body, c);

            c.entities = candidate;
            collisions.push_back(c);
        }
    }
}

bool Geometry::aabbIntersect(AABB* a, AABB* b) {
    if (a->topRight.x < b->bottomLeft.x || a->bottomLeft.x > b->topRight.x ||
        a->topRight.y < b->bottomLeft.y || a->bottomLeft.y > b->topRight.y)
            return false;
    return true;
}

// Optimize for rectangle to check only 2 axis per body  
Collision Geometry::checkPlygonPolygon(RigidBody *a, RigidBody *b) {
    Collision c{};
    c.depth = FLT_MAX;

    for (i32 i = 0; i < a->vertexCount; i++) {
        glm::vec3 edge = a->vertices[(i + 1) % a->vertexCount] - a->vertices[i];
        glm::vec3 axis = glm::normalize(glm::vec3(-edge.y, edge.x, 0.0f));
        pair<f32, f32> ap = projectVerticesOnAxis(a->vertices, a->vertexCount, axis);
        pair<f32, f32> bp = projectVerticesOnAxis(b->vertices, b->vertexCount, axis);

        if (ap.first > bp.second || bp.first > ap.second) {
            c.colided = false;
            return c;
        }

        f32 depth = fmin(ap.second - bp.first, bp.second - ap.first);
        if (depth < c.depth) {
            c.depth = depth;
            c.normal = axis;
        }
    }

    for (i32 i = 0; i < b->vertexCount; i++) {
        glm::vec3 edge = b->vertices[(i + 1) % b->vertexCount] - b->vertices[i];
        glm::vec3 axis = glm::normalize(glm::vec3(-edge.y, edge.x, 0.0f));
        pair<f32, f32> ap = projectVerticesOnAxis(a->vertices, a->vertexCount, axis);
        pair<f32, f32> bp = projectVerticesOnAxis(b->vertices, b->vertexCount, axis);

        if (ap.first > bp.second || bp.first > ap.second) {
            c.colided = false;
            return c;
        }

        f32 depth = fmin(ap.second - bp.first, bp.second - ap.first);
        if (depth < c.depth) {
            c.depth = depth;
            c.normal = axis;
        }
    }

    c.colided = true;

    glm::vec3 ab = b->position - a->position;
    if (glm::dot(ab, c.normal) < 0.0f)
        c.normal = -c.normal;

    return c;
}


pair<f32, f32> Geometry::projectVerticesOnAxis(
    glm::vec3* vertices,
    i32 count,
    glm::vec3 axis
) {
    f32 min = FLT_MAX;
    f32 max = FLT_MIN;

    for (i32 i = 0; i < count; i++) {
        f32 p = glm::dot(vertices[i], axis);
        if (p < min) min = p;
        if (p > max) max = p;
    }

    return pair(min, max);
}

void Geometry::findPolygonPolygonContactPoints(RigidBody &a, RigidBody &b, Collision &c) {
    f32 delta = 0.0005f;
    f32 minSqDistance = FLT_MAX;
    for (i32 i = 0; i < ENTITY_VERTEX_COUNT; i++) {
        glm::vec3 v = a.vertices[i];
        for (i32 j = 0; j < ENTITY_VERTEX_COUNT; j++) {
            PointLineResult res = findClosestPointToLine(v, b.vertices[j], b.vertices[(j + 1) % ENTITY_VERTEX_COUNT]);
            if (closeTo(res.distSq, minSqDistance, delta)) {
                if (!closeTo(&res.cp, &c.cp1, delta)) {
                    c.cp2 = res.cp;
                    c.contactPointsCount = 2;
                }
            }
            else if (res.distSq < minSqDistance) {
                minSqDistance = res.distSq;
                c.cp1 = res.cp;
                c.contactPointsCount = 1;
            }
        }
    }
    for (i32 i = 0; i < ENTITY_VERTEX_COUNT; i++) {
        glm::vec3 v = b.vertices[i];
        for (i32 j = 0; j < ENTITY_VERTEX_COUNT; j++) {
            PointLineResult res = findClosestPointToLine(v, a.vertices[j], a.vertices[(j + 1) % ENTITY_VERTEX_COUNT]);
            if (closeTo(res.distSq, minSqDistance, delta)) {
                if (!closeTo(&res.cp, &c.cp1, delta)) {
                    c.cp2 = res.cp;
                    c.contactPointsCount = 2;
                }
            }
            else if (res.distSq < minSqDistance) {
                minSqDistance = res.distSq;
                c.cp1 = res.cp;
                c.contactPointsCount = 1;
            }
        }
    }

}

PointLineResult Geometry::findClosestPointToLine(glm::vec3 &p, glm::vec3 &a, glm::vec3 &b) {
    glm::vec3 ab = b - a;
    glm::vec3 ap = p - a;
    f32 pr = glm::dot(ap, ab);
    f32 abLengthSq = lengthSq(&ab);
    f32 abSegSq = pr / abLengthSq;

    PointLineResult r{};
    if (abSegSq <= 0.0f)
        r.cp = a;
    else if (abSegSq >= 1.0f)
        r.cp = b;
    else
        r.cp = a + ab * abSegSq;

    r.distSq = distanceSq(&p, &r.cp);

    return r;
}
