#include "physics.h"
#include "geometry.h"
#include "helpers.h"

using std::vector;

void Physics::resolveCollisions(vector<Collision>& collisions, vector<Entity>& entities) {
    for (auto &c: collisions) {
        [[maybe_unused]] Entity &a = entities.at(c.entities.first);
        [[maybe_unused]] Entity &b = entities.at(c.entities.second);

        positionSolver(c, a.body, b.body);
        applyNormalImpulse(c, a.body, b.body);
        applyFrictionImpulse(c, a.body, b.body);
    }
}

void Physics::positionSolver(Collision& c, RigidBody& a, RigidBody& b) {
    if (a.inverseMass == 0.0f) 
        b.position += c.depth * c.normal;
    else if (b.inverseMass == 0.0f)
        a.position -= c.depth * c.normal;
    else {
        a.position -= 0.5f * c.depth * c.normal;
        b.position += 0.5f * c.depth * c.normal;
    }
}

void Physics::applyNormalImpulse(Collision& c, RigidBody& a, RigidBody& b) {
    f32 e = 0.0f;

    for (i32 i = 0; i < c.contactCount; i++) {
        glm::vec3 pa = c.points[i] - a.position;
        glm::vec3 pb = c.points[i] - b.position;

        glm::vec3 pap = glm::vec3(-pa.y, pa.x, 0.0f);
        glm::vec3 pbp = glm::vec3(-pb.y, pb.x, 0.0f);

        glm::vec3 omegaA = a.angularVelocity * pap;
        glm::vec3 omegaB = b.angularVelocity * pbp;

        glm::vec3 va = a.linearVelocity + omegaA; 
        glm::vec3 vb = b.linearVelocity + omegaB; 

        glm::vec3 vRelative = vb - va;

        f32 contactVelocityLen = glm::dot(vRelative, c.normal);

        if (contactVelocityLen > 0.0f)
            continue;

        f32 j = -(1 + e) * contactVelocityLen /
            (a.inverseMass + b.inverseMass +
             pow(glm::dot(pap, c.normal), 2) * a.inverseInertia +
             pow(glm::dot(pbp, c.normal), 2) * b.inverseInertia);

        j /= (f32) c.contactCount;

        c.normalImpulseLen[i] = j;
    }

    for (i32 i = 0; i < c.contactCount; i++) {
        glm::vec3 pa = c.points[i] - a.position;
        glm::vec3 pb = c.points[i] - b.position;

        glm::vec3 impulse = c.normalImpulseLen[i] * c.normal;

        a.linearVelocity -= impulse * a.inverseMass;
        a.angularVelocity -= cross(pa, impulse) * a.inverseInertia;
        b.linearVelocity += impulse * b.inverseMass;
        b.angularVelocity += cross(pb, impulse) * b.inverseInertia;
    }
}

void Physics::applyFrictionImpulse(Collision& c, RigidBody& a, RigidBody& b) {
    glm::vec3 frictionImpulses[2]{};
    f32 staticFriction = (a.staticFriction + b.staticFriction) * 0.5f;
    f32 dynamicFriction = (a.dynamicFriction + b.dynamicFriction) * 0.5f;

    for (i32 i = 0; i < c.contactCount; i++) {
        glm::vec3 pa = c.points[i] - a.position;
        glm::vec3 pb = c.points[i] - b.position;

        glm::vec3 pap = glm::vec3(-pa.y, pa.x, 0.0f);
        glm::vec3 pbp = glm::vec3(-pb.y, pb.x, 0.0f);

        glm::vec3 omegaA = a.angularVelocity * pap;
        glm::vec3 omegaB = b.angularVelocity * pbp;

        glm::vec3 va = a.linearVelocity + omegaA; 
        glm::vec3 vb = b.linearVelocity + omegaB; 

        glm::vec3 vRelative = vb - va;

        glm::vec3 tangent = vRelative - glm::dot(vRelative, c.normal) * c.normal;

        glm::vec3 zero = glm::vec3(0.0f, 0.0f, 0.0f);
        if (closeTo(tangent, zero, DELTA_ERROR))
            continue;
        else
            tangent = glm::normalize(tangent);

        f32 jf = -glm::dot(vRelative, tangent) /
            (a.inverseMass + b.inverseMass +
             pow(glm::dot(pap, tangent), 2) * a.inverseInertia +
             pow(glm::dot(pbp, tangent), 2) * b.inverseInertia);

        jf /= (f32) c.contactCount;

        if (fabs(jf) < c.normalImpulseLen[i] * staticFriction)
            frictionImpulses[i] = jf * tangent;
        else
            frictionImpulses[i] = -c.normalImpulseLen[i] * tangent * dynamicFriction;
    }

    for (i32 i = 0; i < c.contactCount; i++) {
        glm::vec3 pa = c.points[i] - a.position;
        glm::vec3 pb = c.points[i] - b.position;

        a.linearVelocity -= frictionImpulses[i] * a.inverseMass;
        a.angularVelocity -= cross(pa, frictionImpulses[i]) * a.inverseInertia;
        b.linearVelocity += frictionImpulses[i] * b.inverseMass;
        b.angularVelocity += cross(pb, frictionImpulses[i]) * b.inverseInertia;
    }
}
