#ifndef DRONE_H
#define DRONE_H

#include "entity/entity_manager.h"
#include "renderer/render_components.h"
#include "physics/physics_component.h"
#include "renderer/basic_shapes.h"
#include "sensors/sensor.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <array>
#include <vector>
#include <boost/asio/io_context.hpp>  // For io_context
#include "mqtt.h"
#include "asyc_udp.h"

class Drone {
public:
    Drone(const glm::vec3& position, const glm::vec3& size, float mass);
    void create_body(const glm::vec3& position, const glm::vec3& size, float mass);
    void create_propellers(const glm::vec3& position, const glm::vec3& size);

    void update(float dt);  // Physics update
    void setRotorSpeed(int index, float speed);

    // Add a sensor to the drone
 

    // Get the drone's current pose (position and orientation)
    glm::vec3 getPosition() const;
    glm::quat getOrientation() const;
    glm::vec3 getVelocity() const;
    glm::vec3 getAngularVelocity() const;

    Entity drone_entity;

private:
    EntityManager& manager = EntityManager::getInstance();

    std::array<float, 4> rotorSpeeds;
    std::array<glm::vec3, 4> propellerOffsets;
    std::array<Entity, 4> propellerEntities;

    float thrustCoefficient = 10.0f;  // Adjust as needed
    float torqueCoefficient = 0.1f;   // Adjust as needed

};

// // --- Update Physics and Rotor Control ---
// void Drone::update(float dt) {
//     auto physics = manager.getComponent<PhysicsComponent>(drone_entity.getID());
//     auto baseTransform = manager.getComponent<TransformComponent>(drone_entity.getID());
//     if (!physics || !baseTransform) return;

//     btRigidBody* body = physics->body;
//     if (!body) return;

//     float totalYawTorque = 0.0f;

//     for (int i = 0; i < 4; ++i) {
//         // Apply thrust force
//         float thrust = rotorSpeeds[i] * thrustCoefficient;
//         glm::vec3 worldOffset = propellerOffsets[i];
//         glm::vec3 worldPos = baseTransform->position + worldOffset;

//         btVector3 btWorldPos(worldPos.x, worldPos.y, worldPos.z);
//         btVector3 force(0, thrust, 0);

//         body->applyForce(force, btWorldPos - body->getCenterOfMassPosition());

//         // Apply spin-induced yaw torque
//         float spinDir = (i == 0 || i == 3) ? 1.0f : -1.0f;
//         float torque = rotorSpeeds[i] * torqueCoefficient * spinDir;
//         totalYawTorque += torque;

//         // Update visual spin and position of the propeller
//         auto propTransform = manager.getComponent<TransformComponent>(propellerEntities[i].getID());
//         if (propTransform) {
//             float angle = rotorSpeeds[i] * dt * 5.0f; // radians per frame
//             glm::quat spin = glm::angleAxis(angle, glm::vec3(0.0f, 1.0f, 0.0f));
//             propTransform->rotation = spin * propTransform->rotation;
//             propTransform->position = baseTransform->position + propellerOffsets[i];
//         }
//     }

//     // Apply yaw torque
//     body->applyTorque(btVector3(0, totalYawTorque, 0));
// }

// void Drone::update(float dt) {
//     auto physics = manager.getComponent<PhysicsComponent>(drone_entity.getID());
//     auto baseTransform = manager.getComponent<TransformComponent>(drone_entity.getID());
//     if (!physics || !baseTransform) return;

//     btRigidBody* body = physics->body;
//     if (!body) return;

//     float totalYawTorque = 0.0f;

//     // Get orientation of the drone
//     btTransform transform = body->getWorldTransform();
//     btQuaternion btRot = transform.getRotation();
//     glm::quat orientation(btRot.getW(), btRot.getX(), btRot.getY(), btRot.getZ());

//     for (int i = 0; i < 4; ++i) {
//         // Apply thrust force
//         float thrust = rotorSpeeds[i] * thrustCoefficient;
        
//         // Rotate local propeller offset into world space
//         glm::vec3 rotatedOffset = orientation * propellerOffsets[i]; 
//         glm::vec3 worldPos = baseTransform->position + rotatedOffset;

//         btVector3 force(0, thrust, 0);
//         btVector3 relPos(rotatedOffset.x, rotatedOffset.y, rotatedOffset.z);
        
//         // Apply force at position relative to center of mass
//         body->applyForce(force, relPos);

//         // Apply spin-induced yaw torque
//         float spinDir = (i == 0 || i == 3) ? 1.0f : -1.0f;
//         float torque = rotorSpeeds[i] * torqueCoefficient * spinDir;
//         totalYawTorque += torque;

//         // Update propeller visuals
//         auto propTransform = manager.getComponent<TransformComponent>(propellerEntities[i].getID());
//         if (propTransform) {
//             float angle = rotorSpeeds[i] * dt * 5.0f;
//             glm::quat spin = glm::angleAxis(angle, glm::vec3(0.0f, 1.0f, 0.0f));
//             propTransform->rotation = spin * propTransform->rotation;
//             propTransform->position = baseTransform->position + rotatedOffset;
//         }
//     }

//     // Apply total yaw torque
//     body->applyTorque(btVector3(0, totalYawTorque, 0));
// // }
// void Drone::update(float dt) {
//     auto physics = manager.getComponent<PhysicsComponent>(drone_entity.getID());
//     auto baseTransform = manager.getComponent<TransformComponent>(drone_entity.getID());
//     if (!physics || !baseTransform) return;

//     btRigidBody* body = physics->body;
//     if (!body) return;

//     // Get current orientation from physics
//     btTransform transform = body->getWorldTransform();
//     btQuaternion btRot = transform.getRotation();
//     glm::quat orientation(btRot.getW(), btRot.getX(), btRot.getY(), btRot.getZ());

//     float totalYawTorque = 0.0f;

//     for (int i = 0; i < 4; ++i) {
//         float rotorSpeed = rotorSpeeds[i];
//         float thrust = rotorSpeed * thrustCoefficient;

//         // Rotate propeller offset into world space using current orientation
//         glm::vec3 rotatedOffset = orientation * propellerOffsets[i];
//         glm::vec3 worldPos = getPosition() + rotatedOffset;

//         // Apply thrust force at propeller location
//         btVector3 force(0, thrust, 0);
//         btVector3 relPos(rotatedOffset.x, rotatedOffset.y, rotatedOffset.z);
//         body->applyForce(force, relPos);

//         // Calculate yaw torque from rotor spin direction
//         float spinDir = (i == 0 || i == 3) ? 1.0f : -1.0f;
//         float torque = rotorSpeed * torqueCoefficient * spinDir;
//         totalYawTorque += torque;

//         // Update propeller visuals
//         auto propTransform = manager.getComponent<TransformComponent>(propellerEntities[i].getID());
//         if (propTransform) {
//             float angle = rotorSpeed * dt * 5.0f;
//             glm::quat spin = glm::angleAxis(angle, glm::vec3(0.0f, 1.0f, 0.0f));
//             propTransform->rotation = orientation * spin ;
//             propTransform->position = getPosition() + rotatedOffset;
//         }
//     }

//     // Apply accumulated yaw torque to the body
//     body->applyTorque(btVector3(0.0, totalYawTorque, 0));
// }


void Drone::update(float dt) {
    auto physics = manager.getComponent<PhysicsComponent>(drone_entity.getID());
    auto baseTransform = manager.getComponent<TransformComponent>(drone_entity.getID());
    if (!physics || !baseTransform) return;

    btRigidBody* body = physics->body;
    if (!body) return;

    // Get current orientation from physics
    btTransform transform = body->getWorldTransform();
    btQuaternion btRot = transform.getRotation();
    glm::quat orientation(btRot.getW(), btRot.getX(), btRot.getY(), btRot.getZ());

    float totalYawTorque = 0.0f;
    float forwardThrust = 0.0f;
    float rollThrust = 0.0f;
    
    // Loop through the rotors and apply forces
    for (int i = 0; i < 4; ++i) {
        float rotorSpeed = rotorSpeeds[i];
        float thrust = rotorSpeed * thrustCoefficient;

        // Apply differential thrust for pitch control (front/rear)
        if (i == 0 || i == 3) {
            thrust -= forwardThrust;  // Less thrust for front rotors to tilt forward
        } else {
            thrust += forwardThrust;  // More thrust for rear rotors to tilt forward
        }

        // Apply differential thrust for roll control (left/right)
        if (i == 0 || i == 1) {
            thrust -= rollThrust;  // Less thrust for left rotors to roll right
        } else {
            thrust += rollThrust;  // More thrust for right rotors to roll right
        } 
       
        // Rotate propeller offset into world space using current orientation
        glm::vec3 rotatedOffset = orientation * propellerOffsets[i];
        glm::vec3 worldPos = getPosition() + rotatedOffset;

        // Apply thrust force at propeller location
       // btVector3 force(0, thrust, 0);
        glm::vec3 localThrust(0.0f, thrust, 0.0f); // local "up"
        glm::vec3 worldThrust = orientation * localThrust;
        btVector3 force(worldThrust.x, worldThrust.y, worldThrust.z);
        btVector3 relPos(rotatedOffset.x, rotatedOffset.y, rotatedOffset.z);
        body->applyForce(force, relPos);

        // Calculate yaw torque from rotor spin direction
        float spinDir = (i == 0 || i == 3) ? 1.0f : -1.0f;
        float torque = rotorSpeed * torqueCoefficient * spinDir;
        totalYawTorque += torque;

        // Update propeller visuals
        auto propTransform = manager.getComponent<TransformComponent>(propellerEntities[i].getID());
        if (propTransform) {
            float angle = rotorSpeed * dt * 5.0f;
            glm::quat spin = glm::angleAxis(angle, glm::vec3(0.0f, 1.0f, 0.0f));
            propTransform->rotation = orientation * spin;
            propTransform->position = getPosition() + rotatedOffset;
        }
    }

    // Apply accumulated yaw torque to the body
    body->applyTorque(btVector3(0, totalYawTorque, 0));
}

// void Drone::update(float dt) {
//     auto physics = manager.getComponent<PhysicsComponent>(drone_entity.getID());
//     auto baseTransform = manager.getComponent<TransformComponent>(drone_entity.getID());
//     if (!physics || !baseTransform) return;

//     btRigidBody* body = physics->body;
//     if (!body) return;

//     btTransform transform = body->getWorldTransform();
//     btQuaternion btRot = transform.getRotation();
//     glm::quat orientation(btRot.getW(), btRot.getX(), btRot.getY(), btRot.getZ());

//     // Inputs (-1.0 to 1.0)
//     float forwardThrust = pitchInput * maxPitchThrust;
//     float rollThrust    = rollInput  * maxRollThrust;
//     float yawThrust     = yawInput   * maxYawTorque;

//     for (int i = 0; i < 4; ++i) {
//         float rotorSpeed = rotorSpeeds[i];
//         float thrust = rotorSpeed * thrustCoefficient;

//         // Pitch control (front/back)
//         if (i == 0 || i == 3) {  // front
//             thrust -= forwardThrust;
//         } else {                // back
//             thrust += forwardThrust;
//         }

//         // Roll control (left/right)
//         if (i == 0 || i == 1) {  // left
//             thrust -= rollThrust;
//         } else {                // right
//             thrust += rollThrust;
//         }

//         // Clamp to non-negative
//         thrust = std::max(thrust, 0.0f);

//         // Thrust direction: local up rotated into world space
//         glm::vec3 rotatedOffset = orientation * propellerOffsets[i];
//         glm::vec3 worldPos = getPosition() + rotatedOffset;
//         glm::vec3 localThrust(0.0f, thrust, 0.0f);
//         glm::vec3 worldThrust = orientation * localThrust;

//         btVector3 force(worldThrust.x, worldThrust.y, worldThrust.z);
//         btVector3 relPos(rotatedOffset.x, rotatedOffset.y, rotatedOffset.z);
//         body->applyForce(force, relPos);

//         // Apply yaw torque (per rotor)
//         float spinDir = (i == 0 || i == 3) ? 1.0f : -1.0f;
//         float torque = rotorSpeed * torqueCoefficient * spinDir + yawThrust * spinDir;
//         body->applyTorque(btVector3(0, torque, 0));

//         // Update visual rotation
//         auto propTransform = manager.getComponent<TransformComponent>(propellerEntities[i].getID());
//         if (propTransform) {
//             float angle = rotorSpeed * dt * 5.0f;
//             glm::quat spin = glm::angleAxis(angle, glm::vec3(0.0f, 1.0f, 0.0f));
//             propTransform->rotation = orientation * spin;
//             propTransform->position = getPosition() + rotatedOffset;
//         }
//     }
// }



// --- Rotor Control ---
void Drone::setRotorSpeed(int index, float speed) {
    if (index >= 0 && index < 4) {
        rotorSpeeds[index] = speed;
    }
}

// --- Constructor ---
Drone::Drone(const glm::vec3& position, const glm::vec3& size, float mass) {

    drone_entity = Entity();
    manager.addEntity(drone_entity);
    create_body(position,size,mass);
    create_propellers(position,size);

}

void Drone::create_body(const glm::vec3& position, const glm::vec3& size, float mass){
 
    auto transform = std::make_shared<TransformComponent>();
    transform->position = position;
    manager.addComponent(drone_entity.getID(), transform);

    auto renderable = std::make_shared<RenderableComponent>(generateCube(size));
    GLuint colorTexture = createColorTexture(glm::vec3(0.2f, 0.6f, 1.0f), 512, 512);
    TextureComponent texComp;
    texComp.id = colorTexture;
    texComp.type = "texture_diffuse";
    renderable->textures.push_back(texComp);
    renderable->meshes[0].textures.push_back(texComp);
    manager.addComponent(drone_entity.getID(), renderable);

    // Physics setup
    auto physics = std::make_shared<PhysicsComponent>();
    btCollisionShape* shape = new btBoxShape(btVector3(size.x, size.y, size.z));
    btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(
        btQuaternion(0, 0, 0, 1),
        btVector3(position.x, position.y, position.z)
    ));
    btVector3 inertia(0, 0, 0);
    shape->calculateLocalInertia(mass, inertia);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, inertia);
    physics->body = new btRigidBody(rbInfo);
    manager.addComponent(drone_entity.getID(), physics);


}

void Drone::create_propellers(const glm::vec3& position, const glm::vec3& size){
    float halfX = size.x;
    float halfZ = size.z;
    propellerOffsets[0] = glm::vec3(halfX, 0.25f, halfZ); // Front right
    propellerOffsets[1] = glm::vec3(-halfX, 0.25f, halfZ); // Front left
    propellerOffsets[2] = glm::vec3(-halfX, 0.25f, -halfZ); // Back left
    propellerOffsets[3] = glm::vec3(halfX, 0.25f, -halfZ); // Back right

    // Create visual-only propellers
    for (int i = 0; i < 4; ++i) {
        Entity prop = Entity();
        manager.addEntity(prop);

        auto propTransform = std::make_shared<TransformComponent>();
        propTransform->position = position + propellerOffsets[i];
        manager.addComponent(prop.getID(), propTransform);

        auto propRenderable = std::make_shared<RenderableComponent>(generateCube(glm::vec3(0.15f, 0.02f, 0.15f)));
        GLuint propTexture = createColorTexture(glm::vec3(1.0f, 0.0f, 0.0f), 256, 256);
        TextureComponent propTexComp;
        propTexComp.id = propTexture;
        propTexComp.type = "texture_diffuse";
        propRenderable->textures.push_back(propTexComp);
        propRenderable->meshes[0].textures.push_back(propTexComp);
        manager.addComponent(prop.getID(), propRenderable);

        propellerEntities[i] = prop;
    }

    rotorSpeeds.fill(0.0f);
}

// Get position from the physics engine (physics update should handle this)
glm::vec3 Drone::getPosition() const {
    auto physics = manager.getComponent<PhysicsComponent>(drone_entity.getID());
    if (!physics || !physics->body) return glm::vec3(0.0f);

    btVector3 pos = physics->body->getCenterOfMassPosition();
    return glm::vec3(pos.x(), pos.y(), pos.z());
}

// Get orientation from the physics engine
glm::quat Drone::getOrientation() const {
    auto physics = manager.getComponent<PhysicsComponent>(drone_entity.getID());
    if (!physics || !physics->body) return glm::quat();

    btQuaternion quat = physics->body->getOrientation();
    return glm::quat(quat.w(), quat.x(), quat.y(), quat.z());
}

glm::vec3 Drone::getVelocity() const {
    auto physics = manager.getComponent<PhysicsComponent>(drone_entity.getID());
    if (!physics || !physics->body) return glm::vec3(0.0f);

    btVector3 vel = physics->body->getLinearVelocity();
    return glm::vec3(vel.x(), vel.y(), vel.z());
}

glm::vec3 Drone::getAngularVelocity() const {
    auto physics = manager.getComponent<PhysicsComponent>(drone_entity.getID());
    if (!physics || !physics->body) return glm::vec3(0.0f);

    btVector3 angVel = physics->body->getAngularVelocity();
    return glm::vec3(angVel.x(), angVel.y(), angVel.z());
}

#endif // DRONE_H
