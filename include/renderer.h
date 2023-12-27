#pragma once

#include <glm/glm.hpp>
#include "types.h"
#include "state.h"
#include "world.h"

struct VideoEntity {
    f32 *vertices;
    ui32 *indices;
    ui32 indiceCount;
    ui32 shaderProgram;
    ui32 vbo;
    ui32 vao;
    ui32 ebo;
};

struct Renderer {
    VideoEntity quad;
    VideoEntity circle;
    f32 screenWidth;
    f32 screenHeight;
    f32 aspectRatio;
};

Renderer * createRenderer(f32 screenWidth, f32 screenHeight);
void setupScene(Renderer *r);
void rendererCleanup(Renderer *r);
void drawFrame(Renderer *r, Scene *scene, Game *game, World *world);
