#include "body.h"

AABB AABB::fatten(const f32 amount) const {
    glm::vec3 d = glm::vec3(amount, amount, 0.0f);
    return AABB(
        bottomLeft - amount,
        topRight + amount
    );
}

AABB AABB::merge(const AABB &other) const {
    return AABB (
        min(this->bottomLeft, other.bottomLeft),
        max(this->topRight, other.topRight)
    );
}

f32 AABB::perimiter() const {
    f32 p = 2.0f * (topRight.x - bottomLeft.x) + 2.0f * (topRight.y - bottomLeft.y);
    assert(p >= 0.0f);
    return p;
}

bool AABB::contains(AABB& other) const {
    return
        bottomLeft.x <= other.bottomLeft.x &&
        bottomLeft.y <= other.bottomLeft.y &&
        topRight.x >= other.topRight.x &&
        topRight.y >= other.topRight.y;
}

bool AABB::overlaps(AABB& other) const {
    glm::vec3 d1 = bottomLeft - other.topRight;
    glm::vec3 d2 = other.bottomLeft - topRight;

	if (d1.x > 0.0f || d1.y > 0.0f)
		return false;

	if (d2.x > 0.0f || d2.y > 0.0f)
		return false;

	return true;

}

RigidBody::RigidBody(): RigidBody(
    BodyType::RECTANGLE,
    glm::vec3(1.0f, 1.0f, 1.0f),
    false,
    glm::vec3(0.0f, 0.0f, 0.0f),
    0.0f,
    0.55,
    0.2f,
    0.5f
) {}

RigidBody::RigidBody(
    BodyType type,
    glm::vec3 scale,
    bool immovable,
    glm::vec3 initPosition,
    f32 initOrientation,
    f32 staticFriction,
    f32 dynamicFriction,
    f32 restitution
):
    type(type),
    scale(scale),
    position(initPosition),
    prevPosition(initPosition),
    orientation(initOrientation),
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

void RigidBody::reset(glm::vec3 newPosition) {
    acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    linearVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
    position = newPosition;
    prevPosition = newPosition;
    angularVelocity = 0.0f;
    orientation = 0.0f;
    updateAABB();
    transformToWorld();
}

void RigidBody::updateVelocity(f32 dt) {
    if (inverseMass == 0.0f)
        return;

    glm::vec3 gravity = glm::vec3(0.0f, -9.81f, 0.0f);
    // glm::vec3 gravity = glm::vec3(0.0f, 0.0f, 0.0f);
    linearVelocity += (acceleration + gravity) * dt;
}

void RigidBody::updatePosition(f32 dt) {
    if (inverseMass == 0.0f) {
        updateAABB();
        return;
    }

    prevPosition = position;
    position += linearVelocity * dt;
    orientation += angularVelocity * dt;

    transformToWorld();
    updateAABB();
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
