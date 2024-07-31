#include "collision.h"

void Collision::mergeContacts(Collision& c) {
    entities = c.entities;
    bodies = c.bodies;
    normal = c.normal;
    tangent = c.tangent;
    colided = c.colided;
    contactCount = c.contactCount;

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

    for (i32 i = 0; i < contactCount; i++) {
        contacts[i] = c.contacts[i];
    }
}
// void Collision::mergeContacts(Collision& c) {
//     normal = c.normal;
//     for (i32 i = 0; i < c.contactCount; i++) {
//         Contact &newContact = c.contacts[i];
//
//         for (i32 j = 0; j < contactCount; j++) {
//         Contact &oldContact = contacts[j];
//             if (newContact.id == oldContact.id) {
//                 newContact.accNormalImpulse = oldContact.accNormalImpulse;
//                 newContact.accTangentImpulse = oldContact.accTangentImpulse;
//                 newContact.lifeDuration = ++oldContact.lifeDuration;
//                 break;
//             }
//         }
//     }
//
//     contactCount = c.contactCount;
//     for (i32 i = 0; i < contactCount; i++)
//         contacts[i] = c.contacts[i];
// }

void Collision::addContactPoint(Contact c) {
    contacts[contactCount] = c;
    contactCount++;
};

bool Contact::isStable() {
    return lifeDuration > STABLE_CONTACT_MIN_FRAMES;
}
