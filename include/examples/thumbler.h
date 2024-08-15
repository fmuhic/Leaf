#include "examples/example.h"

struct Thumbler: public Example {
    Thumbler(EntitySystem* es, Leaf* leaf): Example(es), leaf(leaf) {}
    void setup();
    void update(f32 elapsed);
    
    private:

    void updateCageSide(glm::vec3 p, RigidBody* body, f32 elapsed);

    Leaf* leaf;
    i32 ground1Id;
    i32 ground2Id;
    i32 ground3Id;
    i32 ground4Id;
};
