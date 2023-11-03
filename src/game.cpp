#include "game.h"

using glm::vec3;

void updateGame(UserInput *input, vec3 *player) {
    if (input->rightPressed)
        player->x += 0.1f;
    if (input->leftPressed)
        player->x -= 0.1f;
    if (input->upPressed)
        player->y += 0.1f;
    if (input->downPressed)
        player->y -= 0.1f;
}
