#include <string>
#include <iostream>
#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "renderer.h"
#include "loader.h"

using std::string;
using std::cout;
using std::endl;

ui32 compileShader(string path, ui32 type) {
    string file = readFromFile(path.c_str()); 
    const char *src = file.c_str();
    ui32 shader = glCreateShader(type);

    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    i32 success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        cout << "Unable to compile shader: " + path << endl << infoLog << endl;
        abort();
    }

    return shader;
}

ui32 createProgram(ui32 vertexShader, ui32 fragmentShader) {
    ui32 program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    i32 success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        cout << "Unable to compile shader program: " << infoLog << endl;
    }

    return program;
}

void createQuadEntity(VideoEntity *quad, ui32 program) {
    const ui32 vert_size = 12;
    quad->vertices = new f32[vert_size]{
         0.5f,  0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };

    const ui32 ind_size = 6;
    quad->indices = new ui32[ind_size]{
        0, 1, 3,
        1, 2, 3
    };

    quad->shaderProgram = program;

    ui32 VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
 
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vert_size * sizeof(f32), quad->vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind_size * sizeof(ui32), quad->indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void *) 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    quad->vao = VAO;
    quad->vbo = VBO;
    quad->ebo = EBO;
}

Renderer * createRenderer(f32 screenWidth, f32 screenHeight, ui32 metersToPixels) {
    Renderer *r = new Renderer {};
    r->screenWidth = screenWidth;
    r->screenHeight = screenHeight;
    r->aspectRatio = screenWidth / screenHeight;
    r->metersToPixels = metersToPixels;

    ui32 quadVertex = compileShader("./src/shaders/simple.vert", GL_VERTEX_SHADER);
    ui32 quadFragment = compileShader("./src/shaders/simple.frag", GL_FRAGMENT_SHADER);
    ui32 quadProgram = createProgram(quadVertex, quadFragment);

    createQuadEntity(&r->quad, quadProgram);

    glDeleteShader(quadVertex);
    glDeleteShader(quadFragment);

    return r;
}

void setupScene(Renderer *r) {
    glViewport(0, 0, r->screenWidth, r->screenHeight);
}

void rendererCleanup(Renderer *r) {
    glDeleteVertexArrays(1, &r->quad.vao);
    glDeleteBuffers(1, &r->quad.vbo);
    glDeleteBuffers(1, &r->quad.ebo);
    glDeleteProgram(r->quad.shaderProgram);

    delete r;
}

void drawFrame(Renderer *r, Camera *c, glm::vec3 *player) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ui32 shaderProgram = r->quad.shaderProgram;
    glUseProgram(shaderProgram);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(player->x * r->metersToPixels, player->y * r->metersToPixels, 0.0f));
    model = glm::scale(model, glm::vec3(100.0f, 100.0f, 1.0f));
    model = glm::rotate(model, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));

    GLint modelLocation = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

    // glm::mat4 camera = glm::lookAt(
    //     glm::vec3(player->x, plyaer->y, 1.0f),
    //     glm::vec3(player->x, player->y, 0.0f),
    //     glm::vec3(0.0f, 1.0f, 0.0f)
    // );
    glm::mat4 camera = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    GLint viewLocation = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(camera * r->metersToPixels));

    glm::mat4 projection = glm::ortho(0.0f, 1000.0f * r->aspectRatio, 0.0f, 1000.0f, -1.0f, 1.0f);
    GLint projectionLocation = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r->quad.ebo);
    glBindVertexArray(r->quad.vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
