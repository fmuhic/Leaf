#pragma once

#include "types.h"
#include "state.h"

struct VideoEntity {
    f32 *vertices;
    ui32 *indices;
    ui32 shaderProgram;
    ui32 vbo;
    ui32 vao;
    ui32 ebo;
};

struct Renderer {
    VideoEntity quad;
    f32 screenWidth;
    f32 screenHeight;
    ui32 metersToPixels;
};

Renderer * createRenderer(f32 screenWidth, f32 screenHeight, ui32 metersToPixels);
void setupScene(Renderer *r);
void rendererCleanup(Renderer *r);
void drawFrame(Renderer *r, Camera *c);
