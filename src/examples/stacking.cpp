#include "examples/stacking.h"

void StackingExample::setup(std::vector<Entity>& entities) {
    createImmovableGround(entities);

    i32 pilarCount = 5;
    i32 height = 15;
    glm::vec3 initPosition = glm::vec3(-8.0f, -6.8f, 0.0f);
    for (i32 i = 0; i < pilarCount; i++)
        createPilar(entities, initPosition + glm::vec3(i * 4.0f, 0.0f, 0.0f), height);
}


void StackingExample::update(
    [[maybe_unused]] std::vector<Entity>& entities,
    [[maybe_unused]] f32 elapsed
) {}

void StackingExample::createImmovableGround(std::vector<Entity>& entities) {
    Entity *e = findFreeEntity(entities);
    if (e == nullptr)
        return;

    e->body = RigidBody(
        BodyType::RECTANGLE,
        glm::vec3(25.0f, 1.0f, 1.0f),
        true,
        glm::vec3(0.0f, -8.0f, 0.0f),
        0.0f,
        0.55f,
        0.2f,
        0.5f
    );
    e->isAlive = true;
    e->color = glm::vec3(1.0f, 1.0f, 1.0f);
}

void StackingExample::createPilar(std::vector<Entity>& entities, glm::vec3 position, i32 height) {
    f32 verticalDistance = 0.05f;
    f32 xOffset = 0.008f;
    for (i32 i = 0; i < height; i++) {
            Entity *e = findFreeEntity(entities);
            if (e == nullptr)
                return;

            e->body = RigidBody(
                BodyType::RECTANGLE,
                glm::vec3(1.0f, 1.0f, 1.0f),
                false,
                position + glm::vec3(
                    pickRand(0, 5) * xOffset,
                    i + i * verticalDistance,
                    0.0f
                ),
                0.0f,
                0.55,
                0.2f,
                0.5f
            );
            e->isAlive = true;
            e->color = COLORS[pickRand(0, COLORS.size() - 1)];
    }
}
