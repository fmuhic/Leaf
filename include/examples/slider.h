#pragma once

#include "examples/example.h"

struct SliderExample: public Example {
    SliderExample(EntitySystem* es): Example(es) {}
    void setup();
    void update(f32 elapsed);

    private:
    f32 lastBlockSpawn = 0.0f;
};
