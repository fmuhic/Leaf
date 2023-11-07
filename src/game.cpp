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

void printVec(std::string name, glm::vec3 p) {
    cout << name << " (x=" << p.x << ", y=" << p.y << ", z=" << p.z << ")\n";
}

void updateGame(Game *game, KeyboardInput *kInput, MouseInput *mInput, glm::vec3 *player) {
    if (kInput->rightPressed)
        player->x += 0.1f;
    if (kInput->leftPressed)
        player->x -= 0.1f;
    if (kInput->upPressed)
        player->y += 0.1f;
    if (kInput->downPressed)
        player->y -= 0.1f;

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
}
