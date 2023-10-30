#pragma once

#include <string>
#include <unordered_map>

#include "types.h"
#include "state.h"

std::string readFromFile(const char *path);

std::unordered_map<std::string, uint32> compileShaders(
    std::unordered_map<std::string,
    std::string> shaderInfo, uint32 type
);

void createShaderPrograms(
    Renderer *renderer,
    std::unordered_map<std::string, uint32> vertexShaders,
    std::unordered_map<std::string, uint32> fragmentShaders
);

void createVertexObjects(Renderer *renderer);
