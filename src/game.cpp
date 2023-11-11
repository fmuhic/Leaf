#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "game.h"
#include "glm/ext/vector_float3.hpp"
#include "helpers.h"
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

i32 i = 0;
void updateGame(f32 dt, Game *game, KeyboardInput *kInput, MouseInput *mInput) {
    game->player.a = glm::vec3(0.0f, 0.0f, 0.0f);

    if (kInput->rightPressed)
        game->player.a += glm::vec3(100.0f, 0.0f, 0.0f);
    if (kInput->leftPressed)
        game->player.a += glm::vec3(-100.0f, 0.0f, 0.0f);
    if (kInput->upPressed)
        game->player.a += glm::vec3(0.0f, 100.0f, 0.0f);
    if (kInput->downPressed)
        game->player.a += glm::vec3(0.0f, -100.0f, 0.0f);

    glm::vec3 p = game->player.p;
    glm::vec3 v = game->player.v;
    game->player.v = game->player.a * dt + v;
    game->player.p = game->player.a * dt * dt * 0.5f + v * dt + p;

    // Poor mans friction
    game->player.v -= game->player.v * 0.05f;
    
    if (mInput->leftClickClicked) {
        i32 eIndex = getFirstFreeEntity(game);
        if (eIndex != -1) {
            EntityType type = (EntityType) pickRand(0, 1);
            Entity *e = &game->entities[eIndex];
            e->type = type;
            e->p = glm::vec3(mInput->position.x, mInput->position.y, 0.0f);
            e->isAlive = true;
            e->scale = pickRand(10, 20) / 10.0f;
            e->color = game->colors[pickRand(0, COLOR_COUNT - 1)];
        }
    }

    for (auto &e: game->entities) {
        if(e.isAlive) {
            glm::vec3 p = e.p;
            glm::vec3 v = e.v;

            glm::vec3 g = glm::vec3(0.0f, -9.81f, 0.0f);
            e.v = (e.a + g) * dt + v;
            e.p = e.a * dt * dt * 0.5f + e.v * dt + p;
        }
    }
}
