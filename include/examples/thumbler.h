#include "examples/example.h"

struct Thumbler: public Example {
    void setup(std::vector<Entity>& entities);
    void update(std::vector<Entity>& entities, f32 elapsed);
    
    private:
    void updateCageSide(glm::vec3 p, RigidBody& body, f32 elapsed);
};
