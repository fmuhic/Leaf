#include "mmath.h"

f32 lengthSq(glm::vec3 *a) {
    return a->x * a->x + a->y * a->y;
}

f32 distanceSq(glm::vec3 *a, glm::vec3 *b) {
    f32 dx = a->x - b->x;
    f32 dy = a->y - b->y;
    return dx * dx + dy * dy;
}

f32 cross(glm::vec3 *a, glm::vec3 *b) {
    return (a->x * b->y) - (a->y * b->x);
}

bool closeTo(f32 a, f32 b, f32 delta) {
    return fabs(a - b) < delta;
}

bool closeTo(glm::vec3 *a, glm::vec3 *b, f32 delta) {
    return closeTo(a->x, b->x, delta) && closeTo(a->y, b->y, delta);
}
