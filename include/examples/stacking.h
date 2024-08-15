#include "entity_system.h"
#include "examples/example.h"

struct StackingExample: public Example {
    StackingExample(EntitySystem* es): Example(es) {}
    void setup();
    void update(f32 elapsed);
    
    private:

    void createPilar(glm::vec3 position, i32 height);
};
