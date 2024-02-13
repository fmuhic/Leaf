#include "examples/example.h"

Entity* Example::findFreeEntity(std::vector<Entity>& entities) {
    for (auto &e: entities) {
        if (!e.isAlive)
            return &e;
    }
    return nullptr;
}
