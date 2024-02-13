#include "examples/example.h"

struct StackingExample: public Example {
    void setup(std::vector<Entity>& entities);
    void update(std::vector<Entity>& entities, f32 elapsed);
    
    private:

    void createPilar(std::vector<Entity>& entities, glm::vec3 position, i32 height);
    void createImmovableGround(std::vector<Entity>& entities);
};
