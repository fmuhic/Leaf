#include <cmath>
#include <iostream>

#include "physics.h"
#include "geometry.h"

using std::vector;

void Physics::resolveCollisions(vector<Collision>& collisions, vector<Entity>& entities, f32 dtInv) {
    for (auto &c: collisions) {
        Entity &a = entities.at(c.entities.first);
        Entity &b = entities.at(c.entities.second);

        prepareContacts(c, a.body, b.body, dtInv);

        for (i32 i = 0; i < correctionCount; i++) {
            applyNormalImpulse(c, a.body, b.body);
            applyFrictionImpulse(c, a.body, b.body);
        }
    }
}

void Physics::prepareContacts(Collision& collision, RigidBody& a, RigidBody& b, f32 dtInv) {
    for (i32 i = 0; i < collision.contactCount; i++) {
        Contact& contact = collision.contacts[i];

        contact.positionCorrection = -positionCorrectionFactor * dtInv * fmin(0.0f, -contact.depth + allowedPenetration);
        contact.friction = (a.dynamicFriction + b.dynamicFriction) * 0.5f;

        glm::vec3 pa = contact.point - a.position;
        glm::vec3 pb = contact.point - b.position;

        glm::vec3 pap = glm::vec3(-pa.y, pa.x, 0.0f);
        glm::vec3 pbp = glm::vec3(-pb.y, pb.x, 0.0f);

        contact.inverseNormalMass = 1.0f / (
            a.inverseMass + b.inverseMass +
            pow(glm::dot(pap, collision.normal), 2) * a.inverseInertia +
            pow(glm::dot(pbp, collision.normal), 2) * b.inverseInertia
        );

        collision.tangent = cross(collision.normal, 1.0f);

        contact.inverseTangentMass = 1.0f / (
            a.inverseMass + b.inverseMass +
            pow(glm::dot(pap, collision.tangent), 2) * a.inverseInertia +
            pow(glm::dot(pbp, collision.tangent), 2) * b.inverseInertia
        );
    }
}

void Physics::applyNormalImpulse(Collision& collision, RigidBody& a, RigidBody& b) {
    for (i32 i = 0; i < collision.contactCount; i++) {
        Contact& contact = collision.contacts[i];
        glm::vec3 pa = contact.point - a.position;
        glm::vec3 pb = contact.point - b.position;

        glm::vec3 contactVelocity =
            b.linearVelocity + cross(b.angularVelocity, pb) -
            a.linearVelocity - cross(a.angularVelocity, pa);

        f32 normalVelocityLen = glm::dot(contactVelocity, collision.normal);

        contact.normalImpulseLen = (-normalVelocityLen + contact.positionCorrection) * contact.inverseNormalMass;
        contact.normalImpulseLen = fmax(contact.normalImpulseLen, 0.0f);
        glm::vec3 impulse = contact.normalImpulseLen * collision.normal; 

        a.linearVelocity -= impulse * a.inverseMass;
        a.angularVelocity -= cross(pa, impulse) * a.inverseInertia;
        b.linearVelocity += impulse * b.inverseMass;
        b.angularVelocity += cross(pb, impulse) * b.inverseInertia;
    }
}

void Physics::applyFrictionImpulse(Collision& collision, RigidBody& a, RigidBody& b) {
    for (i32 i = 0; i < collision.contactCount; i++) {
        Contact& contact = collision.contacts[i];
        glm::vec3 pa = contact.point - a.position;
        glm::vec3 pb = contact.point - b.position;

        glm::vec3 contactVelocity =
            b.linearVelocity + cross(b.angularVelocity, pb) -
            a.linearVelocity - cross(a.angularVelocity, pa);

        f32 jf = -glm::dot(contactVelocity, collision.tangent) * contact.inverseTangentMass;
        float maxFriction = contact.friction * contact.normalImpulseLen;
        jf = clamp(jf, -maxFriction, maxFriction);
        glm::vec3 impulse = jf * collision.tangent;

        a.linearVelocity -= impulse * a.inverseMass;
        a.angularVelocity -= cross(pa, impulse) * a.inverseInertia;
        b.linearVelocity += impulse * b.inverseMass;
        b.angularVelocity += cross(pb, impulse) * b.inverseInertia;
    }
}
