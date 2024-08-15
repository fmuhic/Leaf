#include "examples/stacking.h"
#include "helpers.h"

void StackingExample::setup() {
    entitySystem->createImmovableGround(
        glm::vec3(0.0f, -8.0f, 0.0f),
        glm::vec3(25.0f, 1.0f, 1.0f)
    );

    i32 pilarCount = 5;
    i32 height = 15;
    glm::vec3 initPosition = glm::vec3(-8.0f, -7.0f, 0.0f);
    for (i32 i = 0; i < pilarCount; i++) {
        createPilar(initPosition + glm::vec3(i * 4.0f, 0.0f, 0.0f), height);
    }
}

void StackingExample::update([[maybe_unused]] f32 elapsed) {}

void StackingExample::createPilar(glm::vec3 position, i32 height) {
    f32 verticalDistance = 0.0000f;
    f32 xOffset = 0.008f;
    for (i32 i = 0; i < height; i++) {
        entitySystem->createDynamicBox(
            position + glm::vec3(pickRand(0, 5) * xOffset, i + i * verticalDistance, 0.0f)
        );
    }
}
