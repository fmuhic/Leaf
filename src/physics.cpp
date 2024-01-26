#include <cmath>
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

        contact.aToContact = contact.point - a.position;
        contact.bToContact = contact.point - b.position;

        glm::vec3 pap = glm::vec3(-contact.aToContact.y, contact.aToContact.x, 0.0f);
        glm::vec3 pbp = glm::vec3(-contact.bToContact.y, contact.bToContact.x, 0.0f);

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
        glm::vec3 contactVelocity =
            b.linearVelocity + cross(b.angularVelocity, contact.bToContact) -
            a.linearVelocity - cross(a.angularVelocity, contact.aToContact);

        f32 normalVelocityLen = glm::dot(contactVelocity, collision.normal);

        contact.normalImpulseLen = (-normalVelocityLen + contact.positionCorrection) * contact.inverseNormalMass;
        contact.normalImpulseLen = fmax(contact.normalImpulseLen, 0.0f);
    }

    for (i32 i = 0; i < collision.contactCount; i++) {
        Contact& contact = collision.contacts[i];
        glm::vec3 impulse = contact.normalImpulseLen * collision.normal; 

        a.linearVelocity -= impulse * a.inverseMass;
        a.angularVelocity -= cross(contact.aToContact, impulse) * a.inverseInertia;
        b.linearVelocity += impulse * b.inverseMass;
        b.angularVelocity += cross(contact.bToContact, impulse) * b.inverseInertia;
    }
}

void Physics::applyFrictionImpulse(Collision& collision, RigidBody& a, RigidBody& b) {
    for (i32 i = 0; i < collision.contactCount; i++) {
        Contact& contact = collision.contacts[i];

        glm::vec3 contactVelocity =
            b.linearVelocity + cross(b.angularVelocity, contact.bToContact) -
            a.linearVelocity - cross(a.angularVelocity, contact.aToContact);

        contact.frictionImpulseLen = -glm::dot(contactVelocity, collision.tangent) * contact.inverseTangentMass;

        float maxFriction = contact.friction * contact.normalImpulseLen;
        contact.frictionImpulseLen = clamp(contact.frictionImpulseLen, -maxFriction, maxFriction);
    }

    for (i32 i = 0; i < collision.contactCount; i++) {
        Contact& contact = collision.contacts[i];
        glm::vec3 impulse = contact.frictionImpulseLen * collision.tangent;

        a.linearVelocity -= impulse * a.inverseMass;
        a.angularVelocity -= cross(contact.aToContact, impulse) * a.inverseInertia;
        b.linearVelocity += impulse * b.inverseMass;
        b.angularVelocity += cross(contact.bToContact, impulse) * b.inverseInertia;
    }
}
