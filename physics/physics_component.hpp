
#ifndef PHYSICS_COMPONENT
#define PHYSICS_COMPONENT

#include <btBulletDynamicsCommon.h>
#include "renderer/render_components.hpp"
#include "entity/component.hpp"



struct PhysicsComponent : public Component {
    btRigidBody* body = nullptr; 
    btCollisionShape* shape = nullptr;
    btMotionState* motionState = nullptr; 

    float mass = 1.0f; 

    PhysicsComponent() = default;

    PhysicsComponent(btRigidBody* b, btCollisionShape* s, btMotionState* m, float massVal)
        : body(b), shape(s), motionState(m), mass(massVal) {}

    ~PhysicsComponent() {
        delete body;
        delete shape;
        delete motionState;
    }
};


#endif