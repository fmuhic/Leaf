#pragma once

#include "entity_system.h"
#include "types.h"

struct Example {
    Example(EntitySystem* es): entitySystem(es) {}
    virtual ~Example() {}
    virtual void setup() = 0;
    virtual void update(f32 elapsed) = 0;

    protected:

    EntitySystem* entitySystem;
};
