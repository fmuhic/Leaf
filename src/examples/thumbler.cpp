#include "examples/thumbler.h"
#include "body.h"

void Thumbler::setup(std::vector<Entity>& entities) {
    Entity& e0 = entities[0];
    e0.body = RigidBody(
        BodyType::RECTANGLE,
        glm::vec3(12.0f, 1.0f, 1.0f),
        true,
        glm::vec3(0.0f, -6.0f, 0.0f),
        0.0f,
        0.55,
        0.2f,
        0.5f
    );
    e0.isAlive = true;
    e0.color = glm::vec3(1.0f, 1.0f, 1.0f);

    Entity &e1 = entities[1];
    e1.body = RigidBody(
        BodyType::RECTANGLE,
        glm::vec3(12.0f, 1.0f, 1.0f),
        true,
        glm::vec3(0.0f, 6.0f, 0.0f),
        0.0f,
        0.55,
        0.2f,
        0.5f
    );
    e1.isAlive = true;
    e1.color = glm::vec3(1.0f, 1.0f, 1.0f);

    Entity &e2 = entities[2];
    e2.body = RigidBody(
        BodyType::RECTANGLE,
        glm::vec3(1.0f, 13.0f, 1.0f),
        true,
        glm::vec3(7.0f, 0.0f, 0.0f),
        0.0f,
        0.55,
        0.2f,
        0.5f
    );
    e2.isAlive = true;
    e2.color = glm::vec3(1.0f, 1.0f, 1.0f);

    Entity &e3 = entities[3];
    e3.body = RigidBody(
        BodyType::RECTANGLE,
        glm::vec3(1.0f, 13.0f, 1.0f),
        true,
        glm::vec3(-7.0f, 0.0f, 0.0f),
        0.0f,
        0.55,
        0.2f,
        0.5f
    );
    e3.isAlive = true;
    e3.color = glm::vec3(1.0f, 1.0f, 1.0f);

    f32 offset = 0.20;
    glm::vec3 initPosition = glm::vec3(-3.0f, -3.0f, 0.0f);
    i32 width = 6;
    i32 height = 6;
    for (i32 i = 0; i < width; i++) {
        for (i32 j = 0; j < height; j++) {
            Entity *e = findFreeEntity(entities);
            if (e == nullptr)
                return;

            e->body = RigidBody(
                BodyType::RECTANGLE,
                glm::vec3(1.0f, 1.0f, 1.0f),
                false,
                initPosition + glm::vec3(
                    i + i * offset,
                    j + j * offset,
                    0.0f
                ),
                0.0f,
                0.55,
                0.2f,
                0.5f
            );
            e->isAlive = true;
        }
    }
}

void Thumbler::update(std::vector<Entity>& entities, f32 elapsed) {
    updateCageSide(glm::vec3(0.0f, -6.0f, 0.0f), entities[0].body, elapsed);
    updateCageSide(glm::vec3(0.0f, 6.0f, 0.0f), entities[1].body, elapsed);
    updateCageSide(glm::vec3(6.5f, 0.0f, 0.0f), entities[2].body, elapsed);
    updateCageSide(glm::vec3(-6.5f, 0.0f, 0.0f), entities[3].body, elapsed);
}

void Thumbler::updateCageSide(glm::vec3 p, RigidBody& body, f32 elapsed) {
    glm::mat4& model = body.model;
    model = glm::mat4(1.0f);
    model = glm::rotate(model, elapsed * 0.3f, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, p);
    model = glm::scale(model, glm::vec3(body.scale.x, body.scale.y, body.scale.z));

    body.vertices[0] = model * glm::vec4(0.5f, 0.5f, 0.0f, 1.0f);
    body.vertices[1] = model * glm::vec4(0.5f, -0.5f, 0.0f, 1.0f);
    body.vertices[2] = model * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
    body.vertices[3] = model * glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f);
    body.position = model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    body.model = model;
}
