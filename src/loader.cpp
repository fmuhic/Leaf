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
#include "state.h"
#include "helpers.h"

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

unordered_map<string, uint32> compileShaders(
    unordered_map<string, string> shaderInfo,
    uint32 type
) {
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

void createShaderPrograms(
    Renderer *renderer,
    unordered_map<string, uint32> vertexShaders,
    unordered_map<string, uint32> fragmentShaders
) {
    uint32 shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShaders["simple"]);
    glAttachShader(shaderProgram, fragmentShaders["simple"]);
    glLinkProgram(shaderProgram);

    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER_PROGRAM::COMPILATION_FAILED::REGULAR_PROGRAM\n" << infoLog << std::endl;
    }

    renderer->shaderPrograms[toIndex(ShaderProgram::REGULAR)] = shaderProgram;
}

void createVertexObjects(Renderer *renderer) {
    float vertices[] = {
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
    unsigned int indices[] = {
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };
    
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
 
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    renderer->vbo[toIndex(VBO::QUAD)] = VBO;
    renderer->ebo[toIndex(EBO::QUAD)] = EBO;
    renderer->vao[toIndex(VAO::QUAD)] = VAO;
}
