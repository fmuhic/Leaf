#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

#include "game.h"
#include "helpers.h"
#include "physics.h"
#include "state.h"
#include "const.h"

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
        player.a += glm::vec3(50.0f, 0.0f, 0.0f);
    if (kInput->leftPressed)
        player.a += glm::vec3(-50.0f, 0.0f, 0.0f);
    if (kInput->upPressed)
        player.a += glm::vec3(0.0f, 50.0f, 0.0f);
    if (kInput->downPressed)
        player.a += glm::vec3(0.0f, -50.0f, 0.0f);

    glm::vec3 p = player.p;
    glm::vec3 v = player.v;
    player.v = player.a * dt + v;
    player.p = player.a * dt * dt * 0.5f + v * dt + p;

    // Poor mans friction
    // player.v -= player.v * 0.05f;

    player.transformed = false;
    
    if (mInput->leftClickClicked) {
        i32 eIndex = getFirstFreeEntity(game);
        if (eIndex != -1) {
            EntityType type = (EntityType) pickRand(0, 1);
            Entity *e = &game->entities[eIndex];
            e->isAlive = true;
            e->isStatic = (bool) pickRand(0, 2);
            e->type = type;
            e->p = glm::vec3(mInput->position.x, mInput->position.y, 0.0f);
            e->angle = glm::radians((f32) pickRand(0, 360));
            e->scale = pickRand(10, 20) / 10.0f;
            e->restitution = 0.66f; // Glass
            if (e->isStatic) {
                e->inverseMass = 0.0f;
                e->color = glm::vec3(0.941, 0.953, 0.957);
            }
            else {
                e->color = game->colors[pickRand(0, COLOR_COUNT - 1)];
                if (type == EntityType::ENTITY_CIRCLE)
                    e->inverseMass = 1.0f / e->r * e->r * e->scale * e->scale * PI;
                else if (type == EntityType::ENTITY_QUAD)
                    e->inverseMass = 1.0f / e->scale * e->scale;
            }

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
