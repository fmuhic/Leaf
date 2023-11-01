#pragma once

#include "types.h"

struct VideoEntity {
    f32 *vertices;
    ui32 *indices;
    ui32 shaderProgram;
    ui32 vbo;
    ui32 vao;
    ui32 ebo;
};

struct Renderer
{
    VideoEntity quad;
};

void initRenderer(Renderer *r);
void rendererCleanup(Renderer *r);
void drawFrame(Renderer *r);
