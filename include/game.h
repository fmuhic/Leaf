#pragma once

#include <glm/glm.hpp>
#include "state.h"

void updateGame(Camera *c, Game *game, KeyboardInput *kInput, MouseInput *mInput, glm::vec3 *player);
