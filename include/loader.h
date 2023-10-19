#pragma once

#include <string>
#include <unordered_map>

#include "types.h"

std::string readFromFile(const char *path);

std::unordered_map<std::string, uint32>
compileShaders(std::unordered_map<std::string, std::string> shaderInfo, uint32 type);

