#include "geometry.h"
#include "body.h"
#include "helpers.h"
#include "leaf_math.h"

#define BODY_A_ID 0
#define BODY_B_ID 1

using std::pair;
using std::vector;
using std::fabs;
using glm::vec3;

Geometry::Geometry(i32 maxEntityCount) {
    candidates.reserve(maxEntityCount);
}

void Geometry::reset() {
    candidates.clear();
    collisions.clear();
}

void Geometry::broadPhase(vector<Entity>& entities) {
    candidates.clear();

    // Todo(Fudo): Switch to spatial partitioning
    for (ui32 i = 0; i < entities.size() - 1; ++i) {
        Entity &a = entities.at(i);

        for (ui32 j = i + 1; j < entities.size(); ++j) {
            Entity &b = entities.at(j);

            if (a.body.inverseMass == 0.0f && b.body.inverseMass == 0.0f)
                continue;

            if (!b.isAlive || !a.isAlive || !aabbIntersect(a.body.aabb, b.body.aabb)) {
                collisions.erase(CollisionKey(i, j));
                continue;
            }

            candidates.push_back(CollisionKey(i, j));
        }
    }
}

void Geometry::narrowPhase(std::vector<Entity>& entities) {
    for (auto &candidate: candidates) {
        Entity &a = entities.at(candidate.first);
        Entity &b = entities.at(candidate.second);

        Collision c;
        if (a.body.type == BodyType::RECTANGLE && b.body.type == BodyType::RECTANGLE)
            c = checkPlygonPolygon(a.body, b.body);
        else
            assert(false && "Circles not implemented for now");

        if (c.colided) { 
            c.entities = candidate;
            findContactPoints(a.body, b.body, c);

            auto iter = collisions.find(candidate);
            if (iter == collisions.end())
                collisions.insert(CollisionPair(candidate, c));
            else
                iter->second.mergeContacts(c);
        }
        else
            collisions.erase(candidate);
    }
}

bool Geometry::aabbIntersect(AABB& a, AABB& b) {
    if (
        a.topRight.x < b.bottomLeft.x ||
        a.bottomLeft.x > b.topRight.x ||
        a.topRight.y < b.bottomLeft.y ||
        a.bottomLeft.y > b.topRight.y
    ) {
        return false;
    }

    return true;
}

// Todo(Fudo): Optimize for rectangle to check only 2 axis per body  
Collision Geometry::checkPlygonPolygon(RigidBody &a, RigidBody &b) {
    Collision c{};
    f32 minDepth = FLT_MAX;

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
        if (depth < minDepth) {
            minDepth = depth;
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
        if (depth < minDepth) {
            minDepth = depth;
            c.normal = axis;
        }
    }

    c.colided = true;

    // Make sure normal always points A -> B
    glm::vec3 ab = b.position - a.position;
    if (glm::dot(ab, c.normal) < 0.0f)
        c.normal = -c.normal;

    return c;
}

void Geometry::findContactPoints(RigidBody& a, RigidBody& b, Collision& collision) {
    Edge aEdge = findContactEdge(a.vertices, a.vertexCount, collision.normal);
    Edge bEdge = findContactEdge(b.vertices, b.vertexCount, -collision.normal);

    Edge referenceEdge, incidentEdge;
    i32 bodyId;
    if (fabs(dot(aEdge.second - aEdge.first, collision.normal)) - edgePreferenceDelta <= fabs(dot(bEdge.second - bEdge.first, collision.normal))) {
        bodyId = BODY_A_ID;
        referenceEdge = aEdge;
        incidentEdge = bEdge;
    }
    else {
        bodyId = BODY_B_ID;
        referenceEdge = bEdge;
        incidentEdge = aEdge;
    }

    vec3 referenceVector = normalize(referenceEdge.first - referenceEdge.second);
    f32 offset = dot(referenceEdge.second, referenceVector); 
    EdgePoints edge = clipEdge(incidentEdge.first, incidentEdge.second, referenceVector, offset);

    offset = dot(referenceEdge.first, referenceVector); 
    edge = clipEdge(edge.points[0], edge.points[1], -referenceVector, -offset);

    vec3 referenceNormal = -vec3(-referenceVector.y, referenceVector.x, 0.0f);
    f32 max = dot(referenceEdge.max, referenceNormal);

    if (dot(edge.points[0], referenceNormal) <= max)
        collision.addContactPoint(Contact(
            ContactId(bodyId, incidentEdge.id.first),
            edge.points[0],
            pointLineDistance(edge.points[0], referenceEdge.first, referenceEdge.second)
        ));

    if (dot(edge.points[1], referenceNormal) <= max)
        collision.addContactPoint(Contact(
            ContactId(bodyId, incidentEdge.id.second),
            edge.points[1],
            pointLineDistance(edge.points[1], referenceEdge.first, referenceEdge.second)
        ));
}

EdgePoints Geometry::clipEdge(
    glm::vec3& p1,
    glm::vec3& p2,
    glm::vec3 referenceEdge,
    f32 referenceOffset
) {
    EdgePoints ep{};
    f32 d1 = dot(p1, referenceEdge) - referenceOffset;
    f32 d2 = dot(p2, referenceEdge) - referenceOffset;

    if (d1 >= 0.0f)
        ep.points[ep.count++] = p1;

    if (d2 >= 0.0f)
        ep.points[ep.count++] = p2;

    if (d1 * d2 < 0.0f) {
        vec3 midPoint = ((p2 - p1) * d1 / (d1 - d2)) + p1;
        ep.points[ep.count++] = midPoint;
    }

    assert(ep.count == 2 && "We always need to end up with 2 contact points");

    return ep;
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

    i32 v0Index = modFloor(index - 1, count);
    i32 v1Index = modFloor(index + 1, count);
    vec3 v = vertices[index];
    vec3 v0 = vertices[v0Index];
    vec3 v1 = vertices[v1Index];

    glm::vec3 leftEdge = normalize(v - v1);
    glm::vec3 rightEdge = normalize(v - v0);

    if (dot(rightEdge, normal) <= dot(leftEdge, normal))
        return Edge { EdgeId(v0Index, index), v, v0, v };
    else
        return Edge { EdgeId(index, v1Index), v, v, v1 };
}

void Collision::mergeContacts(Collision& c) {
    normal = c.normal;
    for (i32 i = 0; i < c.contactCount; i++) {
        Contact &newContact = c.contacts[i];

        for (i32 j = 0; j < contactCount; j++) {
        Contact &oldContact = contacts[j];
            if (newContact.id == oldContact.id) {
                newContact.accNormalImpulse = oldContact.accNormalImpulse;
                newContact.accTangentImpulse = oldContact.accTangentImpulse;
                newContact.lifeDuration = ++oldContact.lifeDuration;
                break;
            }
        }
    }

    contactCount = c.contactCount;
    for (i32 i = 0; i < contactCount; i++)
        contacts[i] = c.contacts[i];
}

void Collision::addContactPoint(Contact c) {
    contacts[contactCount] = c;
    contactCount++;
};

bool Contact::isStable() {
    return lifeDuration > STABLE_CONTACT_MIN_FRAMES;
}
