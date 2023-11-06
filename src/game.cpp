#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "game.h"
#include "glm/ext/vector_float3.hpp"
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

void updateGame(Camera *c, Game *game, KeyboardInput *kInput, MouseInput *mInput, glm::vec3 *player) {
    if (kInput->rightPressed)
        player->x += 0.1f;
    if (kInput->leftPressed)
        player->x -= 0.1f;
    if (kInput->upPressed)
        player->y += 0.1f;
    if (kInput->downPressed)
        player->y -= 0.1f;

    if (mInput->leftClickClicked) {
        glm::vec3 fC = glm::vec3(mInput->position.x, mInput->position.y, 0.0f);
        glm::vec3 ndc = glm::vec3(fC.x / 1280.0f, 1.0f - fC.y / 720.0f, fC.z) * 2.0f - 1.0f;

        f32 aspectRatio = 1280.0f / 720.0f;
        glm::mat4 projection = glm::ortho(-1000.0f * aspectRatio, 1000.0f * aspectRatio, -1000.0f, 1000.0f, -1.0f, 1.0f);
        glm::mat4 finalMatrix = glm::inverse(projection * c->focus);

        glm::vec4 worldPosition = finalMatrix * glm::vec4(ndc, 1);

        glm::vec3 win(mInput->position.x, mInput->position.y, 0);
        glm::vec4 viewport(0, 0, 1280, 720);
        glm::vec3 realpos = glm::unProject(win, glm::mat4(1.0f), glm::mat4(1.0f), viewport);
        f32 ar = 1280.0f / 720.0f;
        i32 eIndex = getFirstFreeEntity(game);
        if (eIndex != -1) {
            Entity *e = &game->entities[eIndex];
            e->p = glm::vec3(worldPosition.x / 100.0f, worldPosition.y / 100.0f, 0.0f);
            e->isAlive = true;
            e->scale = 70;
        }
        cout << "Left mouse clicked at (" << worldPosition.x * ar << ", " << worldPosition.y * 10 << ")\n";
        dumpEntites(game, 10);
    }
}
