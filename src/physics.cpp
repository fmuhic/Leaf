#include <algorithm>
#include <iostream>

#include "physics.h"
#include "geometry.h"

using std::vector;

void Physics::resolveCollisions(std::map<CollisionKey, Collision>& collisions, vector<Entity>& entities, f32 dtInv) {
    for (auto& [_, c]: collisions) {
        Entity &a = entities.at(c.entities.first);
        Entity &b = entities.at(c.entities.second);

        prepareContacts(c, a.body, b.body, dtInv);

        for (i32 i = 0; i < correctionCount; i++) {
            applyImpulses(c, a.body, b.body);
        }
    }
}

void Physics::prepareContacts(Collision& collision, RigidBody& a, RigidBody& b, f32 dtInv) {
    for (i32 i = 0; i < collision.contactCount; i++) {
        Contact& contact = collision.contacts[i];

        contact.positionCorrection = -positionCorrectionFactor * dtInv * min(0.0f, -contact.depth + allowedPenetration);
        contact.friction = (a.dynamicFriction + b.dynamicFriction) * 0.5f;

        glm::vec3 ap = contact.point - a.position;
        glm::vec3 bp = contact.point - b.position;

        glm::vec3 pap = glm::vec3(ap.y, ap.x, 0.0f);
        glm::vec3 pbp = glm::vec3(-bp.y, bp.x, 0.0f);

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

        glm::vec3 P = contact.accNormalImpulse * collision.normal + contact.accFrictionImpulse * collision.tangent;
        a.linearVelocity -= P * a.inverseMass;
        a.angularVelocity -= cross(ap, P) * a.inverseInertia;
        b.linearVelocity += P * b.inverseMass;
        b.angularVelocity += cross(bp, P) * b.inverseInertia;
    }
}

void Physics::applyImpulses(Collision& collision, RigidBody& a, RigidBody& b) {
    for (i32 i = 0; i < collision.contactCount; i++) {
        Contact& contact = collision.contacts[i];

        glm::vec3 ap = contact.point - a.position;
        glm::vec3 bp = contact.point - b.position;

        glm::vec3 contactVelocity =
            b.linearVelocity + cross(b.angularVelocity, bp) -
            a.linearVelocity - cross(a.angularVelocity, ap);

        f32 normalVelocityLen = glm::dot(contactVelocity, collision.normal);

        contact.normalImpulseLen = (contact.positionCorrection - normalVelocityLen) * contact.inverseNormalMass;
        f32 oldAccumulatedImpulse = contact.accNormalImpulse;
        contact.accNormalImpulse = max(contact.normalImpulseLen + oldAccumulatedImpulse, 0.0f);
        contact.normalImpulseLen = contact.accNormalImpulse - oldAccumulatedImpulse;

        glm::vec3 normalImpulse = contact.normalImpulseLen * collision.normal; 
        a.linearVelocity -= normalImpulse * a.inverseMass;
        a.angularVelocity -= cross(ap, normalImpulse) * a.inverseInertia;
        b.linearVelocity += normalImpulse * b.inverseMass;
        b.angularVelocity += cross(bp, normalImpulse) * b.inverseInertia;

        // Friction
        contactVelocity =
            b.linearVelocity + cross(b.angularVelocity, bp) -
            a.linearVelocity - cross(a.angularVelocity, ap);

        contact.frictionImpulseLen = -glm::dot(contactVelocity, collision.tangent) * contact.inverseTangentMass;

        f32 maxFriction = contact.friction * contact.accNormalImpulse;
        f32 oldFrictionImpulse = contact.accFrictionImpulse; 
        contact.accFrictionImpulse = clamp(oldFrictionImpulse + contact.frictionImpulseLen, -maxFriction, maxFriction);
        contact.frictionImpulseLen = contact.accFrictionImpulse - oldFrictionImpulse;

        glm::vec3 frictionImpulse = contact.frictionImpulseLen * collision.tangent;

        a.linearVelocity -= frictionImpulse * a.inverseMass;
        a.angularVelocity -= cross(ap, frictionImpulse) * a.inverseInertia;
        b.linearVelocity += frictionImpulse * b.inverseMass;
        b.angularVelocity += cross(bp, frictionImpulse) * b.inverseInertia;
    }
}
