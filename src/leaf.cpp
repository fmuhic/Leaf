#include "leaf.h"
#include "body.h"

Leaf::Leaf() {
    physics = new Physics();
}

Leaf::~Leaf() {
}

void Leaf::step(f32 dt) {
    for (i32 i = bodies.getFirst(); i != ObjectPool<RigidBody>::END; i = bodies.next(i)) {
        RigidBody body = bodies[i];
        body.updateVelocity(dt);
    }
}

i32 Leaf::createBox(BodyConfig config, UserData data) {
    i32 bodyId = bodies.reserve();
    bodies[bodyId] = RigidBody(config, GeometryType::BOX, data);
    bodies[bodyId].verticesId = vertices.reserve(boxVertexCount);
    bodies.debug("Bodies Add");
    vertices.debug("Vertices Add");

    return bodyId;
}

void Leaf::destroyBody(i32 id) {
    RigidBody body = bodies[id];
    vertices.free(body.verticesId);
    bodies.free(id);
    bodies.debug("Bodies Remove");
    vertices.debug("Vertices Remove");

}
