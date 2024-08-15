#include "aabb.h"

AABB AABB::fatten(const f32 amount) const {
    glm::vec3 d = glm::vec3(amount, amount, 0.0f);
    return AABB(
        bottomLeft - amount,
        topRight + amount
    );
}

AABB AABB::merge(const AABB &other) const {
    return AABB (
        min(this->bottomLeft, other.bottomLeft),
        max(this->topRight, other.topRight)
    );
}

f32 AABB::perimiter() const {
    f32 p = 2.0f * (topRight.x - bottomLeft.x) + 2.0f * (topRight.y - bottomLeft.y);
    assert(p >= 0.0f);
    return p;
}

bool AABB::contains(AABB& other) const {
    return
        bottomLeft.x <= other.bottomLeft.x &&
        bottomLeft.y <= other.bottomLeft.y &&
        topRight.x >= other.topRight.x &&
        topRight.y >= other.topRight.y;
}

bool AABB::overlaps(AABB& other) const {
    glm::vec3 d1 = bottomLeft - other.topRight;
    glm::vec3 d2 = other.bottomLeft - topRight;

	if (d1.x > 0.0f || d1.y > 0.0f)
		return false;

	if (d2.x > 0.0f || d2.y > 0.0f)
		return false;

	return true;
}

