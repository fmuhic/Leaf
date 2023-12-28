#pragma once

#include <glm/glm.hpp>
#include "types.h"

f32 lengthSq(glm::vec3 *a);
f32 distanceSq(glm::vec3 *a, glm::vec3 *b);
f32 cross(glm::vec3 *a, glm::vec3 *b);
bool closeTo(f32 a, f32 b, f32 delta);
bool closeTo(glm::vec3 *a, glm::vec3 *b, f32 delta);
