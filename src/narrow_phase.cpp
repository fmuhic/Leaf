#include "narrow_phase.h"
#include "body.h"
#include "leaf_math.h"

#define BODY_A_ID 0
#define BODY_B_ID 1

using std::pair;
using std::vector;
using std::fabs;
using glm::vec3;

void NarrowPhase::checkPairwiseCollision(
    std::vector<RigidBody>& bodies,
    std::map<CollisionKey, Collision>& collisions
) {
    for (auto &[pair, oldCollision]: collisions) {
        RigidBody &a = bodies.at(pair.first);
        RigidBody &b = bodies.at(pair.second);

        Collision collision;
        if (a.type == GeometryType::BOX && b.type == GeometryType::BOX) {
            collision = checkPlygonPolygon(a, b);
        }
        else
            assert(false && "Circles not implemented for now");

        if (collision.colided) {
            findContactPoints(a, b, collision);
            oldCollision.mergeContacts(collision);
        }
        else {
            oldCollision = Collision();
        }
    }
}

// Todo(Fudo): Optimize for rectangle to check only 2 axis per body  
Collision NarrowPhase::checkPlygonPolygon(RigidBody &a, RigidBody &b) {
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

void NarrowPhase::findContactPoints(RigidBody& a, RigidBody& b, Collision& collision) {
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

    if (dot(edge.points[0], referenceNormal) <= max) {
        f32 depth = pointLineDistance(edge.points[0], referenceEdge.first, referenceEdge.second);
        collision.addContactPoint(Contact(
            ContactId(bodyId, incidentEdge.id.first),
            edge.points[0] + referenceNormal * depth,
            depth
        ));
    }

    if (dot(edge.points[1], referenceNormal) <= max) {
        f32 depth = pointLineDistance(edge.points[1], referenceEdge.first, referenceEdge.second);
        collision.addContactPoint(Contact(
            ContactId(bodyId, incidentEdge.id.second),
            edge.points[1] + referenceNormal * depth,
            depth
        ));
    }
}

EdgePoints NarrowPhase::clipEdge(
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

Edge NarrowPhase::findContactEdge(glm::vec3* vertices, i32 count, glm::vec3 normal) {
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
