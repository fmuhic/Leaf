#pragma once
#include "glm/ext/vector_float3.hpp"
#include "types.h"
#include <utility>

#define MAX_CONTACT_POINTS 2
#define STABLE_CONTACT_MIN_FRAMES 150

typedef std::pair<i32, i32> ContactId;
typedef std::pair<i32, i32> CollisionKey;

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
    // remove this
    std::pair<i32, i32> entities;
    std::pair<i32, i32> bodies;
    bool colided = false;
    glm::vec3 normal;
    glm::vec3 tangent;
    i32 contactCount = 0;
    Contact contacts[MAX_CONTACT_POINTS];

    void mergeContacts(Collision& c);
    void addContactPoint(Contact c);
};

typedef std::pair<CollisionKey, Collision> CollisionPair;
