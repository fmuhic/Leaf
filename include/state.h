#pragma once

#include <cstdint>
#include <unordered_map>
#include <string>

#include "types.h"

#define SHADER_PROGRAM_COUNT 100

enum class ShaderProgram
{
    REGULAR = 0
};

struct State {
};

struct Rendeder 
{
    uint32_t shaderPrograms[SHADER_PROGRAM_COUNT];
};
