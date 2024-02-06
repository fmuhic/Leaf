#include "physics.h"
#include "geometry.h"

using std::vector;

void Physics::resolveCollisions(std::map<CollisionKey, Collision>& collisions, vector<Entity>& entities, f32 dtInv) {
    for (auto& [_, c]: collisions) {
        Entity &a = entities.at(c.entities.first);
        Entity &b = entities.at(c.entities.second);

        prepareContacts(c, a.body, b.body, dtInv);
    }

    for (i32 i = 0; i < correctionCount; i++) {
        for (auto& [_, c]: collisions) {
            Entity &a = entities.at(c.entities.first);
            Entity &b = entities.at(c.entities.second);

            applyTangentImpulse(c, a.body, b.body);
            applyNormalImpulse(c, a.body, b.body);
        }
    }
}

void Physics::applyNormalImpulse(Collision& collision, RigidBody& a, RigidBody& b) {
    for (i32 i = 0; i < collision.contactCount; i++) {
        Contact& contact = collision.contacts[i];

        glm::vec3 ap = contact.point - a.position;
        glm::vec3 bp = contact.point - b.position;

        glm::vec3 contactVelocity =
            b.linearVelocity + cross(b.angularVelocity, bp) -
            a.linearVelocity - cross(a.angularVelocity, ap);

        f32 normalVelocityLen = glm::dot(contactVelocity, collision.normal);

        f32 normalImpulseLen = (-normalVelocityLen + contact.positionCorrection) * contact.inverseNormalMass;
        f32 oldNormalImpulse = contact.accNormalImpulse;
        contact.accNormalImpulse = max(normalImpulseLen + oldNormalImpulse, 0.0f);
        normalImpulseLen = contact.accNormalImpulse - oldNormalImpulse;

        glm::vec3 Jn = normalImpulseLen * collision.normal; 
        a.linearVelocity -= Jn * a.inverseMass;
        a.angularVelocity -= cross(ap, Jn) * a.inverseInertia;
        b.linearVelocity += Jn * b.inverseMass;
        b.angularVelocity += cross(bp, Jn) * b.inverseInertia;
    }
}
void Physics::applyTangentImpulse(Collision& collision, RigidBody& a, RigidBody& b) {
    for (i32 i = 0; i < collision.contactCount; i++) {
        Contact& contact = collision.contacts[i];

        glm::vec3 ap = contact.point - a.position;
        glm::vec3 bp = contact.point - b.position;

        glm::vec3 contactVelocity =
            b.linearVelocity + cross(b.angularVelocity, bp) -
            a.linearVelocity - cross(a.angularVelocity, ap);

        f32 tangentImpulseLen = -glm::dot(contactVelocity, collision.tangent) * contact.inverseTangentMass;

        f32 maxFriction = contact.friction * contact.accNormalImpulse;
        f32 oldFrictionImpulse = contact.accTangentImpulse; 
        contact.accTangentImpulse = clamp(oldFrictionImpulse + tangentImpulseLen, -maxFriction, maxFriction);
        tangentImpulseLen = contact.accTangentImpulse - oldFrictionImpulse;

        glm::vec3 Jt = tangentImpulseLen * collision.tangent;

        a.linearVelocity -= Jt * a.inverseMass;
        a.angularVelocity -= cross(ap, Jt) * a.inverseInertia;
        b.linearVelocity += Jt * b.inverseMass;
        b.angularVelocity += cross(bp, Jt) * b.inverseInertia;
    }
}

void Physics::prepareContacts(Collision& collision, RigidBody& a, RigidBody& b, f32 dtInv) {
    for (i32 i = 0; i < collision.contactCount; i++) {
        Contact& contact = collision.contacts[i];

        contact.positionCorrection = -positionCorrectionFactor * dtInv * min(0.0f, -contact.depth + allowedPenetration);
        contact.friction = (a.dynamicFriction + b.dynamicFriction) / 2.0f;

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

        glm::vec3 J = contact.accNormalImpulse * collision.normal + contact.accTangentImpulse * collision.tangent;
        a.linearVelocity -= J * a.inverseMass;
        a.angularVelocity -= cross(ap, J) * a.inverseInertia;
        b.linearVelocity += J * b.inverseMass;
        b.angularVelocity += cross(bp, J) * b.inverseInertia;
    }
}
