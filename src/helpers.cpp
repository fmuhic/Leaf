#include <random>
#include <assert.h>
#include "helpers.h"

ui32 pickRand(ui32 min, ui32 max) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(min, max);

    return dist6(rng);
}

void printVec(std::string name, glm::vec3 p) {
    std::cout << name << " (x=" << p.x << ", y=" << p.y << ", z=" << p.z << ")\n";
}

f32 max(f32 *xs, i32 count) {
    assert(count > 0);
    f32 max = xs[0];
    for (i32 i = 1; i < count; ++i) {
        if (xs[i] > max)
            max = xs[i];
    }
    return max;
}

f32 min(f32 *xs, i32 count) {
    assert(count > 0);
    f32 min = xs[0];
    for (i32 i = 1; i < count; ++i) {
        if (xs[i] < min)
            min = xs[i];
    }
    return min;
}
