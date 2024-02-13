#pragma once

#include <vector>
#include "entity.h"
#include "types.h"

struct Example {
    virtual ~Example() {}
    virtual void setup(std::vector<Entity>& entities) = 0;
    virtual void update(std::vector<Entity>& entities, f32 elapsed) = 0;

    protected:

    Entity* findFreeEntity(std::vector<Entity>& entities);
};
