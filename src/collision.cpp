#include "collision.h"

void Collision::mergeContacts(Collision& c) {
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

    normal = c.normal;
    contactCount = c.contactCount;
    colided = c.colided;
    for (i32 i = 0; i < contactCount; i++) {
        contacts[i] = c.contacts[i];
    }
}

void Collision::addContactPoint(Contact c) {
    contacts[contactCount] = c;
    contactCount++;
};

bool Contact::isStable() {
    return lifeDuration > STABLE_CONTACT_MIN_FRAMES;
}
