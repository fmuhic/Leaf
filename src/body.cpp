#include "body.h"
#include "helpers.h"

RigidBody::RigidBody(): RigidBody(
    BodyType::RECTANGLE,
    glm::vec3(1.0f, 1.0f, 1.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    false,
    0.55,
    0.35f,
    0.5f
) {}

RigidBody::RigidBody(
    BodyType type,
    glm::vec3 scale,
    glm::vec3 initPosition,
    bool immovable,
    f32 staticFriction,
    f32 dynamicFriction,
    f32 restitution
):
    type(type),
    scale(scale),
    position(initPosition),
    staticFriction(staticFriction),
    dynamicFriction(dynamicFriction),
    restitution(restitution)
{
    switch (type) {
        case BodyType::RECTANGLE: {
            mass = scale.x * scale.y;
            inertia = 1.0f / 12.0f * mass * (scale.x * scale.x + scale.y * scale.y);
            vertexCount = 4;
        } break;

        case BodyType::CIRCLE: {
            mass = scale.x * scale.x;
            inertia = 0.5f * mass * scale.x * scale.x;
            vertexCount = 0;
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

    transformToWorld();
    updateAABB();
}

void RigidBody::reset() {
    acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    linearVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    angularVelocity = 0.0f;
    orientation = 0.0f;
}

void RigidBody::step(f32 dt) {
    if (inverseMass == 0.0f)
        return;

    glm::vec3 gravity = glm::vec3(0.0f, -9.81f, 0.0f);
    linearVelocity += (acceleration + gravity) * dt;

    transformToWorld();
    updateAABB();
}

void RigidBody::updatePosition(f32 dt) {
    position += linearVelocity * dt;
    orientation += angularVelocity * dt;
}

void RigidBody::transformToWorld() {
    model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, orientation, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(scale.x, scale.y, scale.z));

    switch (type) {
        case BodyType::RECTANGLE: {
            vertices[0] = model * glm::vec4(0.5f, 0.5f, 0.0f, 1.0f);
            vertices[1] = model * glm::vec4(0.5f, -0.5f, 0.0f, 1.0f);
            vertices[2] = model * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
            vertices[3] = model * glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f);
        } break;

        case BodyType::CIRCLE: {
            vertices[0] = model * glm::vec4(0.5f, 0.5f, 0.0f, 1.0f);
        } break;
    }
}

void RigidBody::updateAABB() {
    switch (type) {
        case BodyType::RECTANGLE: {
            f32 xMin = vertices[0].x;
            f32 xMax = vertices[0].x;
            f32 yMin = vertices[0].y;
            f32 yMax = vertices[0].y;

            for (i32 i = 1; i < vertexCount; i++) {
                glm::vec3 v = vertices[i];
                if (v.x < xMin) xMin = v.x;
                if (v.x > xMax) xMax = v.x;
                if (v.y < yMin) yMin = v.y;
                if (v.y > yMax) yMax = v.y;
            }

            aabb.bottomLeft = glm::vec3(xMin, yMin, 0.0f);
            aabb.topRight = glm::vec3(xMax, yMax, 0.0f);
        } break;

        case BodyType::CIRCLE: {
            aabb.topRight = position + vertices[0];
            aabb.bottomLeft = position - vertices[0];
        } break;
    }
}
