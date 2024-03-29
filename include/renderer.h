#pragma once

#include <glm/glm.hpp>
#include "types.h"
#include "game.h"

struct Scene {
    glm::mat4 camera;
    glm::vec3 cameraFollow;
    glm::mat4 projection;
    glm::mat4 inverseProjectionView;
};

struct VideoEntity {
    ~VideoEntity() {
        delete vertices;
        delete indices;
    }

    f32 *vertices;
    ui32 *indices;
    ui32 indiceCount;
    ui32 shaderProgram;
    ui32 vbo;
    ui32 vao;
    ui32 ebo;
};

struct Renderer {
    Renderer(f32 width, f32 height);
    ~Renderer();

    void draw(Scene &scene, Game &game);

    VideoEntity quad;
    VideoEntity circle;
    f32 screenWidth;
    f32 screenHeight;
    f32 aspectRatio;

    private:

    void drawEntity(f32 program, VideoEntity &e, Scene &scene, glm::mat4 &model, glm::vec3 &color);
    void createRectangleEntity(ui32 program);
    void createCircleEntity(ui32 program, f32 radius, i32 pointCount);
    glm::vec3 pickContactColor(i32 contactLifeDuration);

    glm::vec3 green = glm::vec3(0.1569f, 0.7059f, 0.3882f);
    glm::vec3 yellow = glm::vec3(0.9451f, 0.7686f, 0.0588f);
    glm::vec3 orange = glm::vec3(0.9529f, 0.6118f, 0.0706f);
    glm::vec3 red = glm::vec3(0.9059f, 0.298f, 0.2353f);
};
