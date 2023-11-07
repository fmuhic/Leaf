#pragma once

#include <iostream>
#include <glm/glm.hpp>

#include "types.h"

template <typename T>
ui32 toIndex(T value) {
    return (ui32) value;
}

ui32 pickRand(ui32 min, ui32 max);
void printVec(std::string name, glm::vec3 p);
