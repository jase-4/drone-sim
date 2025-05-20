
#ifndef PHYSICS_COMPONENT
#define PHYSICS_COMPONENT

#include <btBulletDynamicsCommon.h>
#include "renderer/render_components.h"
#include "entity/component.h"



struct PhysicsComponent : public Component {
    btRigidBody* body = nullptr; // Pointer to the Bullet rigid body
    btCollisionShape* shape = nullptr; // Pointer to the collision shape
    btMotionState* motionState = nullptr; // Pointer to motion state

    float mass = 1.0f; // Default mass

    PhysicsComponent() = default;

    PhysicsComponent(btRigidBody* b, btCollisionShape* s, btMotionState* m, float massVal)
        : body(b), shape(s), motionState(m), mass(massVal) {}

    ~PhysicsComponent() {
        // Cleanup Bullet pointers (depends on how your engine manages physics)
        delete body;
        delete shape;
        delete motionState;
    }
};


#endif