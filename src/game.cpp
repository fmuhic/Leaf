#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "game.h"
#include "helpers.h"
#include "physics.h"
#include "state.h"

using std::cout;

i32 getFirstFreeEntity(Game *game) {
    for (i32 i = 0; i < ENTITY_COUNT; i++) 
        if (!game->entities[i].isAlive)
            return i;
    return -1;
}

void dumpEntites(Game *game, i32 n) {
    for (i32 i = 0; i < n; i++) {
        Entity e = game->entities[i];
        cout << "Entity: alive=" << e.isAlive << ", p=(" << e.p.x << ", " << e.p.y << ",0)\n"; 
    }
}

void updateGame(f32 dt, Game *game, KeyboardInput *kInput, MouseInput *mInput) {
    Entity &player = game->entities[0];
    player.a = glm::vec3(0.0f, 0.0f, 0.0f);

    if (kInput->rightPressed)
        player.a += glm::vec3(100.0f, 0.0f, 0.0f);
    if (kInput->leftPressed)
        player.a += glm::vec3(-100.0f, 0.0f, 0.0f);
    if (kInput->upPressed)
        player.a += glm::vec3(0.0f, 100.0f, 0.0f);
    if (kInput->downPressed)
        player.a += glm::vec3(0.0f, -100.0f, 0.0f);

    glm::vec3 p = player.p;
    glm::vec3 v = player.v;
    player.v = player.a * dt + v;
    player.p = player.a * dt * dt * 0.5f + v * dt + p;

    // Poor mans friction
    player.v -= player.v * 0.05f;

    player.transformed = false;
    
    if (mInput->leftClickClicked) {
        i32 eIndex = getFirstFreeEntity(game);
        if (eIndex != -1) {
            EntityType type = (EntityType) pickRand(0, 1);
            // EntityType type = EntityType::ENTITY_QUAD;
            Entity *e = &game->entities[eIndex];
            e->isAlive = true;
            e->type = type;
            e->p = glm::vec3(mInput->position.x, mInput->position.y, 0.0f);
            e->angle = glm::radians((f32) pickRand(0, 360));
            e->scale = pickRand(10, 20) / 10.0f;
            e->color = game->colors[pickRand(0, COLOR_COUNT - 1)];
        }
    }

    for (i32 i = 1; i < ENTITY_COUNT; ++i) {
        Entity &e = game->entities[i];
        if(e.isAlive) {
            glm::vec3 p = e.p;
            glm::vec3 v = e.v;

            // glm::vec3 g = glm::vec3(0.0f, -9.81f, 0.0f);
            glm::vec3 g = glm::vec3(0.0f, 0.0f, 0.0f);
            e.v = (e.a + g) * dt + v;
            e.p = e.a * dt * dt * 0.5f + e.v * dt + p;
        }
        e.transformed = false;
    }

    checkCollisions(game);
}
