#include "geometry.h"
#include "body.h"
#include "leaf_math.h"

using std::pair;
using std::vector;
using std::fabs;
using glm::vec3;

Geometry::Geometry(i32 maxEntityCount) {
    candidates.reserve(maxEntityCount);
    collisions.reserve(maxEntityCount);
}

void Geometry::broadPhase(vector<Entity>& entities) {
    candidates.clear();

    // Switch to spatial partitioning
    for (ui32 i = 0; i < entities.size() - 1; ++i) {
        Entity &a = entities.at(i);
        if (!a.isAlive)
            continue;

        for (ui32 j = i + 1; j < entities.size(); ++j) {
            Entity &b = entities.at(j);
            if (!b.isAlive)
                continue;

            if (a.body.inverseMass == 0.0f && b.body.inverseMass == 0.0f)
                continue;

            if (!aabbIntersect(a.body.aabb, b.body.aabb))
                continue;

            candidates.push_back(std::pair(i, j));
        }
    }
}

void Geometry::narrowPhase(std::vector<Entity>& entities) {
    collisions.clear();

    for (auto &candidate: candidates) {
        Entity &a = entities.at(candidate.first);
        Entity &b = entities.at(candidate.second);

        Collision c;
        if (a.body.type == BodyType::RECTANGLE && b.body.type == BodyType::RECTANGLE)
            c = checkPlygonPolygon(a.body, b.body);
        else
            assert(false && "Invalid object type");

        if (c.colided) { 
            findContactPoints(a.body, b.body, c);
            c.entities = candidate;
            collisions.push_back(c);
        }
    }
}

bool Geometry::aabbIntersect(AABB& a, AABB& b) {
    if (
        a.topRight.x < b.bottomLeft.x ||
        a.bottomLeft.x > b.topRight.x ||
        a.topRight.y < b.bottomLeft.y ||
        a.bottomLeft.y > b.topRight.y
    )
        return false;

    return true;
}

// Optimize for rectangle to check only 2 axis per body  
Collision Geometry::checkPlygonPolygon(RigidBody &a, RigidBody &b) {
    Collision c{};
    c.depth = FLT_MAX;

    for (i32 i = 0; i < a.vertexCount; i++) {
        glm::vec3 edge = a.vertices[(i + 1) % a.vertexCount] - a.vertices[i];
        glm::vec3 axis = glm::normalize(glm::vec3(-edge.y, edge.x, 0.0f));
        pair<f32, f32> ap = projectVerticesOnAxis(a.vertices, a.vertexCount, axis);
        pair<f32, f32> bp = projectVerticesOnAxis(b.vertices, b.vertexCount, axis);

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

    for (i32 i = 0; i < b.vertexCount; i++) {
        glm::vec3 edge = b.vertices[(i + 1) % b.vertexCount] - b.vertices[i];
        glm::vec3 axis = glm::normalize(glm::vec3(-edge.y, edge.x, 0.0f));
        pair<f32, f32> ap = projectVerticesOnAxis(a.vertices, a.vertexCount, axis);
        pair<f32, f32> bp = projectVerticesOnAxis(b.vertices, b.vertexCount, axis);

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

    glm::vec3 ab = b.position - a.position;
    if (glm::dot(ab, c.normal) < 0.0f)
        c.normal = -c.normal;

    return c;
}

void Geometry::findContactPoints(RigidBody& a, RigidBody& b, Collision& c) {
    Edge aEdge = findContactEdge(a.vertices, a.vertexCount, c.normal);
    Edge bEdge = findContactEdge(b.vertices, b.vertexCount, -c.normal);

    Edge referenceEdge, incidentEdge;
    if (fabs(dot(aEdge.second - aEdge.first, c.normal)) <= fabs(dot(bEdge.second - bEdge.first, c.normal))) {
        referenceEdge = aEdge;
        incidentEdge = bEdge;
    } else {
        referenceEdge = bEdge;
        incidentEdge = aEdge;
    }

    vec3 referenceVector = normalize(referenceEdge.first - referenceEdge.second);
    f32 offset = dot(referenceEdge.second, referenceVector); 
    Contact contact = clipEdge(incidentEdge.first, incidentEdge.second, referenceVector, offset);

    offset = dot(referenceEdge.first, referenceVector); 
    contact = clipEdge(contact.points[0], contact.points[1], -referenceVector, -offset);

    vec3 referenceNormal = -vec3(-referenceVector.y, referenceVector.x, 0.0f);
    f32 max = dot(referenceEdge.max, referenceNormal);

    c.contactCount = 0;
    if (dot(contact.points[0], referenceNormal) <= max) {
        c.points[c.contactCount++] = contact.points[0];
    }

    if (dot(contact.points[1], referenceNormal) <= max) {
        c.points[c.contactCount++] = contact.points[1];
    }
}

Contact Geometry::clipEdge(
    glm::vec3& p1,
    glm::vec3& p2,
    glm::vec3 referenceEdge,
    f32 referenceOffset
){
    Contact c{};
    f32 d1 = dot(p1, referenceEdge) - referenceOffset;
    f32 d2 = dot(p2, referenceEdge) - referenceOffset;

    if (d1 >= 0.0f)
        c.points[c.count++] = p1;

    if (d2 >= 0.0f)
        c.points[c.count++] = p2;

    if (d1 * d2 < 0.0f) {
        vec3 midPoint = ((p2 - p1) * d1 / (d1 - d2)) + p1;
        c.points[c.count++] = midPoint;
    }
    assert(c.count == 2 && "We always need to end up with 2 contact points");

    return c;
}

Edge Geometry::findContactEdge(glm::vec3* vertices, i32 count, glm::vec3 normal) {
    assert(count > 0 && "Minimum vertex count should be 1");
    f32 maxProjection = -FLT_MAX;
    i32 index = -1;
    for (i32 i = 0; i < count; i++) {
        f32 projection = glm::dot(vertices[i], normal);
        if (projection > maxProjection) {
            maxProjection = projection;
            index = i;
        }
    }

    vec3 v = vertices[index];
    vec3 v0 = vertices[modFloor(index - 1, count)];
    vec3 v1 = vertices[modFloor(index + 1, count)];

    glm::vec3 leftEdge = normalize(v - v1);
    glm::vec3 rightEdge = normalize(v - v0);

    if (dot(rightEdge, normal) <= dot(leftEdge, normal))
        return Edge { v, v0, v };
    else
        return Edge { v, v, v1 };
}
