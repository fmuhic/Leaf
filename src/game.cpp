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

void updateGame(f32 dt, Game *game, KeyboardInput *kInput, MouseInput *mInput) {
    game->player.a = glm::vec3(0.0f, 0.0f, 0.0f);

    if (kInput->rightPressed)
        game->player.a += glm::vec3(100000.0f, 0.0f, 0.0f);
    if (kInput->leftPressed)
        game->player.a += glm::vec3(-100000.0f, 0.0f, 0.0f);
    if (kInput->upPressed)
        game->player.a += glm::vec3(0.0f, 100000.0f, 0.0f);
    if (kInput->downPressed)
        game->player.a += glm::vec3(0.0f, -100000.0f, 0.0f);

    game->player.v += game->player.a * dt * 0.5f;
    game->player.p += game->player.v * dt * dt * 0.5f;

    // Poor mans friction
    game->player.v -= game->player.v * 0.045f;
    
    printVec("player p", game->player.p);
    printVec("player v", game->player.v);
    printVec("player a", game->player.a);

    if (mInput->leftClickClicked) {
        i32 eIndex = getFirstFreeEntity(game);
        if (eIndex != -1) {
            Entity *e = &game->entities[eIndex];
            e->p = glm::vec3(mInput->position.x, mInput->position.y, 0.0f);
            e->isAlive = true;
            e->scale = 70;
            e->color = game->colors[pickRand(0, COLOR_COUNT)];
        }
    }

    for (auto &e: game->entities) {
        if(e.isAlive) {
            glm::vec3 g = glm::vec3(0.0f, -900.81f, 0.0f);
            e.v += (e.a + g) * dt * 0.5f;
            e.p += e.v * dt * dt * 0.5f;

            // Poor mans friction
            // e.v -= e.v * 0.005f;
        }
    }
}
