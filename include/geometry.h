#pragma once

#include <vector>
#include <map>

#include "types.h"
#include "const.h"
#include "entity.h"
#include "body.h"
#include "leaf_math.h"

#define STABLE_CONTACT_MIN_FRAMES 150
#define MAX_CONTACT_POINTS 2
#define COLLISION_COUNT ENTITY_COUNT * 4

typedef std::pair<i32, i32> EdgeId;
typedef std::pair<i32, i32> ContactId;

struct EdgePoints {
    i32 count = 0;
    glm::vec3 points[2];
};

struct Contact {
    Contact() {}
    Contact(ContactId id, glm::vec3 point, f32 depth):
        id(id), point(point), depth(depth) {} 

    bool isStable();

    ContactId id;
    i32 lifeDuration = 0;
    glm::vec3 point;
    f32 depth;

    f32 accNormalImpulse = 0.0f;
    f32 accTangentImpulse = 0.0f;

    f32 inverseNormalMass = 0.0f;
    f32 inverseTangentMass = 0.0f;
    f32 positionCorrection = 0.0f;

    f32 friction = 0.0f;
};

struct Collision {
    std::pair<i32, i32> entities;
    bool colided = false;
    glm::vec3 normal;
    glm::vec3 tangent;
    i32 contactCount = 0;
    Contact contacts[MAX_CONTACT_POINTS];

    void mergeContacts(Collision& c);
    void addContactPoint(Contact c);
};

struct Edge {
    EdgeId id;
    glm::vec3 max;
    glm::vec3 first;
    glm::vec3 second;
};

typedef std::pair<i32, i32> CollisionKey;
typedef std::pair<CollisionKey, Collision> CollisionPair;

struct Geometry {
    Geometry(i32 maxEntityCount);

    void broadPhase(std::vector<Entity>& entities);
    void narrowPhase(std::vector<Entity>& entities);
    void reset();

    std::map<CollisionKey, Collision> collisions;

    private:

    bool aabbIntersect(AABB& a, AABB& b);
    Collision checkPlygonPolygon(RigidBody& a, RigidBody& b);
    void findPolygonPolygonContactPoints(RigidBody& a, RigidBody& b, Collision& c);
    void findContactPoints(RigidBody& a, RigidBody& b, Collision& c);
    Edge findContactEdge(glm::vec3* vertices, i32 count, glm::vec3 normal);
    EdgePoints clipEdge(glm::vec3& first, glm::vec3& second, glm::vec3 referenceEdge, f32 referenceOffset);

    std::vector<std::pair<i32, i32>> candidates; 

    f32 edgePreferenceDelta = 0.005f;
};
