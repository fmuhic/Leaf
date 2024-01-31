#include "leaf_math.h"
#include "helpers.h"
#include <cfloat>
#include <iostream>
#include <math.h>

i32 modFloor(i32 num, i32 max) {
    return ((num % max) + max) % max;
}

f32 lengthSq(glm::vec3 &a) {
    return a.x * a.x + a.y * a.y;
}

f32 distanceSq(glm::vec3 &a, glm::vec3 &b) {
    f32 dx = a.x - b.x;
    f32 dy = a.y - b.y;
    return dx * dx + dy * dy;
}

f32 cross(glm::vec3 &a, glm::vec3 &b) {
    return a.x * b.y - a.y * b.x;
}

glm::vec3 cross(glm::vec3 &v, f32 a) {
    return glm::vec3(a * v.y, -a * v.x, 0.0f);
}

glm::vec3 cross(f32 a, glm::vec3 &v) {
    return glm::vec3(-a * v.y, a * v.x, 0.0f);
}

bool closeTo(f32 a, f32 b, f32 delta) {
    return fabs(a - b) < delta;
}

bool closeTo(glm::vec3 &a, glm::vec3 &b, f32 delta) {
    return closeTo(a.x, b.x, delta) && closeTo(a.y, b.y, delta);
}

f32 min(f32 a, f32 b) {
    return a < b ? a : b;
}

f32 max(f32 a, f32 b) {
    return a > b ? a : b;
}

f32 clamp(f32 val, f32 low, f32 high) {
	return max(low, min(val, high));
}


f32 pointLineDistance(glm::vec3 &p, glm::vec3 &a, glm::vec3 &b) {
    glm::vec3 ab = normalize(b - a);
    glm::vec3 ap = p - a;
    f32 pProjection = glm::dot(ap, ab);

    glm::vec3 ppp = a + ab * pProjection;
    glm::vec3 res = p - ppp;
    return glm::length(res);
}


std::pair<f32, f32> projectVerticesOnAxis(glm::vec3* vertices, i32 count, glm::vec3 axis) {
    f32 min = FLT_MAX;
    f32 max = FLT_MIN;

    for (i32 i = 0; i < count; i++) {
        f32 p = glm::dot(vertices[i], axis);
        if (p < min) min = p;
        if (p > max) max = p;
    }

    return std::pair(min, max);
}
