#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <glad/glad.h>

#include "loader.h"
#include "types.h"

using std::string;
using std::cout;
using std::endl;
using std::unordered_map;
using std::ifstream;
using std::stringstream;

string readFromFile(const char *path) {
    ifstream stream(path);
    stringstream buffer;
    buffer << stream.rdbuf();
    return buffer.str();
}

unordered_map<string, uint32> compileShaders(unordered_map<string, string> shaderInfo, uint32 type) {
    unordered_map<string, uint32> result;
    GLint success;
    GLchar infoLog[512];

    for (auto &it: shaderInfo) {
        const char *shaderSrcPath = it.second.c_str();
        string file = readFromFile(shaderSrcPath); 
        const char *src = file.c_str();

        uint32 shader = glCreateShader(type);

        glShaderSource(shader, 1, &src, NULL);
        glCompileShader(shader);

        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            cout << "ERROR::SHADER::COMPILATION_FAILED::" + it.second << endl << infoLog << endl;
            abort();
        }
        result.insert({ it.first, shader});
    }

    return result;
}
