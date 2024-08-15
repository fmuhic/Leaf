#include "game.h"

Game::Game(Leaf* leaf, EntitySystem* entitySystem) {
    this->leaf = leaf;
    this->entitySystem = entitySystem;
}

Game::~Game() {
    delete example;
}

void Game::reset() {
    entitySystem->reset();
}

void Game::changeScene(Example *newExample) {
    reset();
    delete example;
    example = newExample;
    example->setup();
}

void Game::update(f32 dt, f32 elapsed, Input &input) {
    processInput(input);
    updateLogic(elapsed);
    leaf->step(dt);
}

void Game::updateLogic(f32 elapsed) {
    example->update(elapsed);

    for (i32 i = 0; i < (i32) entitySystem->entities.size(); ++i) {
        Entity& e = entitySystem->entities[i];
        if (!e.isAlive) continue;
        
        RigidBody* b = leaf->getBody(e.bodyId);
        if (b->position.x < -50.0f || b->position.y < -50.0f) {
            entitySystem->destroyEntity(i);
        }
    }
}

void Game::processInput(Input &input) {
    if (input.clicked(MouseButton::LEFT)) {
        entitySystem->createDynamicBox(glm::vec3(input.position.x, input.position.y, 0.0f));
    }
}
