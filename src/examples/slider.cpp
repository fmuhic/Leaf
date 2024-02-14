#include "examples/slider.h"
#include "helpers.h"

void SliderExample::setup(std::vector<Entity>& entities) {
    Entity *e = findFreeEntity(entities);
    if (e == nullptr)
        return;

    e->body = RigidBody(
        BodyType::RECTANGLE,
        glm::vec3(20.0f, 1.0f, 1.0f),
        true,
        glm::vec3(-4.5f, 5.0f, 0.0f),
        glm::radians(-15.0f),
        0.55,
        0.2f,
        0.5f
    );
    e->isAlive = true;
    e->color = glm::vec3(1.0f, 1.0f, 1.0f);

    e = findFreeEntity(entities);
    if (e == nullptr)
        return;

    e->body = RigidBody(
        BodyType::RECTANGLE,
        glm::vec3(20.0f, 1.0f, 1.0f),
        true,
        glm::vec3(4.0f, -3.0f, 0.0f),
        glm::radians(15.0f),
        0.55,
        0.2f,
        0.5f
    );
    e->isAlive = true;
    e->color = glm::vec3(1.0f, 1.0f, 1.0f);
}

void SliderExample::update(std::vector<Entity>& entities, [[maybe_unused]] f32 elapsed) {
    if (elapsed - lastBlockSpawn < 1.0f)
        return;

    lastBlockSpawn = elapsed;
    Entity *e = findFreeEntity(entities);
    if (e == nullptr)
        return;

    e->body = RigidBody(
        BodyType::RECTANGLE,
        glm::vec3(pickRand(7, 12) / 10.0f, pickRand(7, 12) / 10.0f, 1.0f),
        false,
        glm::vec3(-6.0f, 10.0f, 0.0f),
        glm::radians(pickRand(0, 36) * 10.0f),
        0.55,
        0.2f,
        0.5f
    );
    e->isAlive = true;
    e->color = COLORS[pickRand(0, COLORS.size() - 1)];
}
