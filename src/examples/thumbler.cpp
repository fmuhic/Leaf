#include "examples/thumbler.h"

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
    // Refactor this
    Entity& e0 = entities[0];
    glm::mat4& model = e0.body.model;
    model = glm::mat4(1.0f);
    model = glm::rotate(model, elapsed * 0.2f, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(0.0f, -6.0f, 0.0f));
    model = glm::scale(model, glm::vec3(e0.body.scale.x, e0.body.scale.y, e0.body.scale.z));

    e0.body.vertices[0] = model * glm::vec4(0.5f, 0.5f, 0.0f, 1.0f);
    e0.body.vertices[1] = model * glm::vec4(0.5f, -0.5f, 0.0f, 1.0f);
    e0.body.vertices[2] = model * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
    e0.body.vertices[3] = model * glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f);
    e0.body.position = model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    e0.body.model = model;

    Entity& e1 = entities[1];
    glm::mat4 model1 = e1.body.model;
    model1 = glm::mat4(1.0f);
    model1 = glm::rotate(model1, elapsed * 0.2f, glm::vec3(0.0f, 0.0f, 1.0f));
    model1 = glm::translate(model1, glm::vec3(0.0f, 6.0f, 0.0f));
    model1 = glm::scale(model1, glm::vec3(e1.body.scale.x, e1.body.scale.y, e1.body.scale.z));

    e1.body.vertices[0] = model1 * glm::vec4(0.5f, 0.5f, 0.0f, 1.0f);
    e1.body.vertices[1] = model1 * glm::vec4(0.5f, -0.5f, 0.0f, 1.0f);
    e1.body.vertices[2] = model1 * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
    e1.body.vertices[3] = model1 * glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f);
    e1.body.position = model1 * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    e1.body.model = model1;

    Entity& e2 = entities[2];
    glm::mat4 model2 = e1.body.model;
    model2 = glm::mat4(1.0f);
    model2 = glm::rotate(model2, elapsed * 0.2f, glm::vec3(0.0f, 0.0f, 1.0f));
    model2 = glm::translate(model2, glm::vec3(6.5f, 0.0f, 0.0f));
    model2 = glm::scale(model2, glm::vec3(e2.body.scale.x, e2.body.scale.y, e2.body.scale.z));

    e2.body.vertices[0] = model2 * glm::vec4(0.5f, 0.5f, 0.0f, 1.0f);
    e2.body.vertices[1] = model2 * glm::vec4(0.5f, -0.5f, 0.0f, 1.0f);
    e2.body.vertices[2] = model2 * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
    e2.body.vertices[3] = model2 * glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f);
    e2.body.position = model2 * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    e2.body.model = model2;

    Entity& e3 = entities[3];
    glm::mat4 model3 = e1.body.model;
    model3 = glm::mat4(1.0f);
    model3 = glm::rotate(model3, elapsed * 0.2f, glm::vec3(0.0f, 0.0f, 1.0f));
    model3 = glm::translate(model3, glm::vec3(-6.5f, 0.0f, 0.0f));
    model3 = glm::scale(model3, glm::vec3(e2.body.scale.x, e2.body.scale.y, e2.body.scale.z));

    e3.body.vertices[0] = model3 * glm::vec4(0.5f, 0.5f, 0.0f, 1.0f);
    e3.body.vertices[1] = model3 * glm::vec4(0.5f, -0.5f, 0.0f, 1.0f);
    e3.body.vertices[2] = model3 * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
    e3.body.vertices[3] = model3 * glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f);
    e3.body.position = model3 * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    e3.body.model = model3;
}
