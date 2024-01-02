#pragma once

#include <glm/glm.hpp>
#include "types.h"

struct PointLineResult {
    glm::vec3 cp;
    f32 distSq;
};
PointLineResult findClosestPointToLine(glm::vec3 &p, glm::vec3 &a, glm::vec3 &b); 
std::pair<f32, f32> projectVerticesOnAxis(glm::vec3* vertices, i32 count, glm::vec3 axis);

f32 lengthSq(glm::vec3 &a);
f32 distanceSq(glm::vec3 &a, glm::vec3 &b);
f32 cross(glm::vec3 &a, glm::vec3 &b);
bool closeTo(f32 a, f32 b, f32 delta);
bool closeTo(glm::vec3 &a, glm::vec3 &b, f32 delta);



