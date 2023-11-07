#include <random>
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

