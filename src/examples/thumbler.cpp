#include "examples/thumbler.h"
#include "body.h"

void Thumbler::setup() {
    ground1Id = entitySystem->createImmovableGround(
        glm::vec3(0.0f, -6.0f, 0.0f),
        glm::vec3(12.0f, 1.0f, 1.0f)
    );
    ground2Id = entitySystem->createImmovableGround(
        glm::vec3(0.0f, 6.0f, 0.0f),
        glm::vec3(12.0f, 1.0f, 1.0f)
    );
    ground3Id = entitySystem->createImmovableGround(
        glm::vec3(0.0f, -7.0f, 0.0f),
        glm::vec3(1.0f, 13.0f, 1.0f)
    );
    ground4Id = entitySystem->createImmovableGround(
        glm::vec3(-7.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 13.0f, 1.0f) 
    );
    f32 offset = 0.05;
    f32 boxWidth = 0.3f;
    glm::vec3 initPosition = glm::vec3(-3.2f, -3.7f, 0.0f);
    i32 width = 20;
    i32 height = 20;
    for (i32 i = 0; i < width; i++) {
        for (i32 j = 0; j < height; j++) {
            entitySystem->createDynamicBox(
                initPosition + glm::vec3(i * boxWidth + i * offset, j * boxWidth + j * offset, 0.0f),
                glm::vec3(boxWidth, boxWidth, 0.0f)
            );
        }
    }
}

void Thumbler::update(f32 elapsed) {
    Entity& g1 = entitySystem->entities[ground1Id];
    RigidBody* rb1 = leaf->getBody(g1.bodyId);
    updateCageSide(glm::vec3(0.0f, -6.0f, 0.0f), rb1, elapsed);

    Entity& g2 = entitySystem->entities[ground2Id];
    RigidBody* rb2 = leaf->getBody(g2.bodyId);
    updateCageSide(glm::vec3(0.0f, 6.0f, 0.0f), rb2, elapsed);

    Entity& g3 = entitySystem->entities[ground3Id];
    RigidBody* rb3 = leaf->getBody(g3.bodyId);
    updateCageSide(glm::vec3(6.5f, 0.0f, 0.0f), rb3, elapsed);

    Entity& g4 = entitySystem->entities[ground4Id];
    RigidBody* rb4 = leaf->getBody(g4.bodyId);
    updateCageSide(glm::vec3(-6.5f, 0.0f, 0.0f), rb4, elapsed);
}

void Thumbler::updateCageSide(glm::vec3 p, RigidBody* body, f32 elapsed) {
    glm::mat4& model = body->model;
    model = glm::mat4(1.0f);
    model = glm::rotate(model, elapsed * 0.3f, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(body->scale.x, body->scale.y, body->scale.z));
    model = glm::translate(model, p);

    body->vertices[0] = model * glm::vec4(0.5f, 0.5f, 0.0f, 1.0f);
    body->vertices[1] = model * glm::vec4(0.5f, -0.5f, 0.0f, 1.0f);
    body->vertices[2] = model * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
    body->vertices[3] = model * glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f);
    body->oldPosition = body->position;
    body->position = model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
