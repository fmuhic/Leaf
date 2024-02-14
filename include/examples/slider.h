#pragma once

#include "examples/example.h"

struct SliderExample: public Example {
    void setup(std::vector<Entity>& entities);
    void update(std::vector<Entity>& entities, f32 elapsed);

    private:
    f32 lastBlockSpawn = 0.0f;
};
