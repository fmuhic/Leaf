#include <string>
#include <assert.h>
#include <iostream>
#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "body.h"
#include "helpers.h"
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

Renderer::Renderer(f32 width, f32 height) {
    screenWidth = width;
    screenHeight = height;
    aspectRatio = width / height;

    glViewport(0, 0, width, height);

    ui32 vertex = compileShader("./src/shaders/simple.vert", GL_VERTEX_SHADER);
    ui32 fragment = compileShader("./src/shaders/simple.frag", GL_FRAGMENT_SHADER);
    ui32 program = createProgram(vertex, fragment);

    createCircleEntity(program, 0.5f, 32);
    createRectangleEntity(program);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Renderer::~Renderer() {
    glDeleteVertexArrays(1, &quad.vao);
    glDeleteBuffers(1, &quad.vbo);
    glDeleteBuffers(1, &quad.ebo);
    glDeleteProgram(quad.shaderProgram);
}

void Renderer::createCircleEntity(ui32 program, f32 r, i32 pointCount) {
    assert(pointCount > 2 && "We need at least 3 points to approximate circle");
    f32 baseAngle = 360.0f / pointCount;
    const i32 triangleCount = pointCount - 2;
    
    circle.vertices = new f32[pointCount * 3];
    for (i32 i = 0; i < pointCount; ++i) {
        f32 angle = glm::radians(baseAngle * i);
        circle.vertices[i * 3] = r * glm::cos(angle);
        circle.vertices[i * 3 + 1] = r * glm::sin(angle);
        circle.vertices[i * 3 + 2] = 0.0f;
    }

    circle.indiceCount = triangleCount * 3;
    circle.indices = new ui32[circle.indiceCount];
    for (i32 i = 0; i < triangleCount; ++i) {
        circle.indices[i * 3] = 0;
        circle.indices[i * 3 + 1] = i + 1;
        circle.indices[i * 3 + 2] = i + 2;
    }

    circle.shaderProgram = program;

    ui32 VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
 
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, pointCount * 3 * sizeof(f32), circle.vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, circle.indiceCount * sizeof(ui32), circle.indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void *) 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    circle.vao = VAO;
    circle.vbo = VBO;
    circle.ebo = EBO;
}

void Renderer::createRectangleEntity(ui32 program) {
    const ui32 vertexCount = 12;
    quad.vertices = new f32[vertexCount] {
         0.5f,  0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };

    quad.indiceCount = 6;
    quad.indices = new ui32[quad.indiceCount] {
        0, 1, 3,
        1, 2, 3
    };

    quad.shaderProgram = program;

    ui32 VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
 
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(f32), quad.vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, quad.indiceCount * sizeof(ui32), quad.indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void *) 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    quad.vao = VAO;
    quad.vbo = VBO;
    quad.ebo = EBO;
}

void Renderer::drawEntity(f32 program, VideoEntity &e, Scene &scene, glm::mat4 &model, glm::vec3 &color) {
    GLint modelLocation = glGetUniformLocation(program, "model");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

    GLint viewLocation = glGetUniformLocation(program, "view");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(scene.camera));

    GLint projectionLocation = glGetUniformLocation(program, "projection");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(scene.projection));

    GLint vertexColorLocation = glGetUniformLocation(program, "vertexColor");
    glUniform4f(vertexColorLocation, color.x, color.y, color.z, 1.0f);

    glDrawElements(GL_TRIANGLES, e.indiceCount, GL_UNSIGNED_INT, 0);
}

void Renderer::draw(Scene &scene, Game &game) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // scene->camera = glm::lookAt(
    //     glm::vec3(scene->cameraFollow.x, scene->cameraFollow.y, 1.0f),
    //     glm::vec3(scene->cameraFollow.x, scene->cameraFollow.y, 0.0f),
    //     glm::vec3(0.0f, 1.0f, 0.0f)
    // );
    scene.camera = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    ui32 shaderProgram = quad.shaderProgram;
    glUseProgram(quad.shaderProgram);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad.ebo);
    glBindVertexArray(quad.vao);
    for (auto &e: game.entities) {
        if (e.isAlive && e.body.type == BodyType::RECTANGLE) 
            drawEntity(shaderProgram, quad, scene, e.body.model, e.color);
    }

    // For debugging
    for (auto& [_, c]: game.geometry->collisions) {
        for (i32 i = 0; i < c.contactCount; i++) {
            glm::vec3 contactColor = pickContactColor(c.contacts[i].lifeDuration);
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, c.contacts[i].point);
            model = glm::scale(model, glm::vec3(0.1f, 0.1f, 1.0f));
            drawEntity(shaderProgram, quad, scene, model, contactColor);
        }
    }

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r->circle.ebo);
    // glBindVertexArray(r->circle.vao);
    // for (auto e: game->entities) {
    //     if (e.isAlive && e.type == EntityType::CIRCLE)
    //         drawEntity(shaderProgram, &r->circle, scene, &e.model, &e.color);
    // }
    // glBindVertexArray(0);
    // glBindBuffer(GL_ARRAY_BUFFER, 0); 
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    //
    //
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r->quad.ebo);
    // glBindVertexArray(r->quad.vao);
    // for (auto e: game->entities) {
    //     if (e.isAlive && e.type == EntityType::RECTANGLE) 
    //         drawEntity(shaderProgram, &r->quad, scene, &e.model, &e.color);
    // }
}

glm::vec3 Renderer::pickContactColor(i32 contactLifeDuration) {
    if (contactLifeDuration < 5)
        return red;
    else if (contactLifeDuration >= 5 && contactLifeDuration < 10)
        return orange;
    else if (contactLifeDuration >= 10 && contactLifeDuration < 50)
        return yellow;
    return green;
}
