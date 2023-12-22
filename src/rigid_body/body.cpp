#include "rigid_body/body.h"
#include "state.h"

Body::Body(
    BodyType type,
    glm::vec3 initPosition,
    glm::vec3 scale,
    bool immovable = false,
    f32 staticFriction = 0.55,
    f32 dynamicFriction = 0.35f,
    f32 restitution = 0.5f
):
    position(initPosition),
    staticFriction(staticFriction),
    dynamicFriction(dynamicFriction),
    restitution(restitution)
{
    switch (type) {
        case BodyType::RECTANGLE: {
            mass = scale.x * scale.y;
            inertia = 1.0f / 12.0f * mass * (scale.x * scale.x + scale.y * scale.y);
        } break;

        case BodyType::CIRCLE: {
            mass = scale.x * scale.x;
            inertia = 0.5f * mass * scale.x * scale.x;
        } break;
    }

    if (immovable) {
        inverseMass = 0.0f;
        inverseInertia = 0.0f;
    }
    else {
        inverseMass = 1.0f / mass;
        inverseInertia = 1.0f / inertia;
    }
}

void Body::step() {
}
