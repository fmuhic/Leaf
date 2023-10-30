#pragma once

#include <cstdint>
#include <unordered_map>
#include <string>

#include "types.h"

#define SHADER_PROGRAM_COUNT 100
#define VBO_COUNT 100
#define VAO_COUNT 100
#define EBO_COUNT 100

enum class ShaderProgram: uint32
{
    REGULAR = 0
};

enum class VBO: uint32
{
    QUAD = 0
};

enum class VAO: uint32
{
    QUAD = 0
};

enum class EBO: uint32
{
    QUAD = 0
};

struct State {
};

struct Renderer 
{
    uint32 shaderPrograms[SHADER_PROGRAM_COUNT];
    uint32 vbo[VBO_COUNT];
    uint32 vao[VAO_COUNT];
    uint32 ebo[EBO_COUNT];
};
