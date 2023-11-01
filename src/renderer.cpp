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

    GLint success;
    GLchar infoLog[512];
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

    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << "Unable to compile shader program: " << infoLog << endl;
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
    f32 aspectRatio = r->screenWidth / r->screenHeight;
    glViewport(0, 0, r->screenWidth * aspectRatio, r->screenHeight * aspectRatio);
}

void rendererCleanup(Renderer *r) {
    glDeleteVertexArrays(1, &r->quad.vao);
    glDeleteBuffers(1, &r->quad.vbo);
    glDeleteBuffers(1, &r->quad.ebo);
    glDeleteProgram(r->quad.shaderProgram);
}

void drawFrame(Renderer *r, Camera *c) {
    // Needs cleanup
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ui32 shaderProgram = r->quad.shaderProgram;
    glUseProgram(shaderProgram);

    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 camera = glm::lookAt(
        glm::vec3(position.x, position.y, 0.05f),
        glm::vec3(position.x, position.y, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    f32 metersToPixels = 100;

    // model = glm::translate(model, glm::vec3(300.0f, 300.0f, 0.0f));
    model = glm::rotate(model, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(300.0f, 300.0f, 1.0f));

    GLint modelMatrix = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, glm::value_ptr(model));

    GLint viewMatrix = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(viewMatrix, 1, GL_FALSE, glm::value_ptr(camera * metersToPixels));

    glm::mat4 projection = glm::ortho(-1280.0f, 1280.0f, -720.0f, 720.0f, -10.0f, 10.0f);

    GLint projectionMatrix = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionMatrix, 1, GL_FALSE, glm::value_ptr(projection));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r->quad.ebo);
    glBindVertexArray(r->quad.vao); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    // glDrawArrays(GL_TRIANGLES, 0, 3);
    // glBindVertexArray(0); // no need to unbind it every time 
}
