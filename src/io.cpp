#include <fstream>
#include <sstream>

#include "io.h"

std::string readFromFile(const char *path) {
    std::ifstream stream(path);
    std::stringstream buffer;
    buffer << stream.rdbuf();
    return buffer.str();
}
