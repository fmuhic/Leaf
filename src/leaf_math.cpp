#include "leaf_math.h"

f32 lengthSq(glm::vec3 &a) {
    return a.x * a.x + a.y * a.y;
}

f32 distanceSq(glm::vec3 &a, glm::vec3 &b) {
    f32 dx = a.x - b.x;
    f32 dy = a.y - b.y;
    return dx * dx + dy * dy;
}

f32 cross(glm::vec3 &a, glm::vec3 &b) {
    return (a.x * b.y) - (a.y * b.x);
}

bool closeTo(f32 a, f32 b, f32 delta) {
    return fabs(a - b) < delta;
}

bool closeTo(glm::vec3 &a, glm::vec3 &b, f32 delta) {
    return closeTo(a.x, b.x, delta) && closeTo(a.y, b.y, delta);
}

PointLineResult findClosestPointToLine(glm::vec3 &p, glm::vec3 &a, glm::vec3 &b) {
    glm::vec3 ab = b - a;
    glm::vec3 ap = p - a;
    f32 pr = glm::dot(ap, ab);
    f32 abLengthSq = lengthSq(ab);
    f32 abSegSq = pr / abLengthSq;

    PointLineResult r{};
    if (abSegSq <= 0.0f)
        r.cp = a;
    else if (abSegSq >= 1.0f)
        r.cp = b;
    else
        r.cp = a + ab * abSegSq;

    r.distSq = distanceSq(p, r.cp);

    return r;
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
