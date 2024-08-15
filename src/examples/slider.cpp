#include "examples/slider.h"
#include "helpers.h"

void SliderExample::setup() {
    entitySystem->createImmovableGround(
        glm::vec3(-5.0f, 5.0f, 0.0f),
        glm::vec3(18.0f, 1.0f, 1.0f),
        glm::radians(-20.0f)
    );
    entitySystem->createImmovableGround(
        glm::vec3(4.0f, -3.5f, 0.0f),
        glm::vec3(20.0f, 1.0f, 1.0f),
        glm::radians(20.0f)
    );
}

void SliderExample::update([[maybe_unused]]f32 elapsed) {
    if (elapsed - lastBlockSpawn < 1.0f) return;
    lastBlockSpawn = elapsed;

    entitySystem->createDynamicBox(
        glm::vec3(-6.0f, 10.0f, 0.0f),
        glm::vec3(pickRand(7, 12) / 10.0f, pickRand(7, 12) / 10.0f, 1.0f),
        glm::radians(pickRand(0, 36) * 10.0f) 
    );
}
