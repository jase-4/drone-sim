#include "drone.hpp"

#include <glm/gtc/quaternion.hpp>
#include <algorithm>


Drone::Drone(const glm::vec3& position, const glm::vec3& size, float mass) {
    drone_entity = Entity();
    manager.addEntity(drone_entity);
    create_body(position, size, mass);
    create_propellers(position, size);
}


void Drone::setRotorSpeed(int index, float speed) {
    if (index >= 0 && index < 4) {
        rotorSpeeds[index] = speed;
    }
}

void Drone::setFrontRightRotorSpeed(float speed) { setRotorSpeed(0, speed); }
void Drone::setFrontLeftRotorSpeed(float speed)  { setRotorSpeed(1, speed); }
void Drone::setBackLeftRotorSpeed(float speed)   { setRotorSpeed(2, speed); }
void Drone::setBackRightRotorSpeed(float speed)  { setRotorSpeed(3, speed); }

// --- Body and Propellers ---
void Drone::create_body(const glm::vec3& position, const glm::vec3& size, float mass) {
    auto transform = std::make_shared<TransformComponent>();
    transform->position = position;
    manager.addComponent(drone_entity.getID(), transform);

    auto renderable = std::make_shared<RenderableComponent>(generateCube(size));
    GLuint colorTexture = createColorTexture(glm::vec3(0.2f, 0.6f, 1.0f), 512, 512);
    TextureComponent texComp{ colorTexture, "texture_diffuse" };
    renderable->textures.push_back(texComp);
    renderable->meshes[0].textures.push_back(texComp);
    manager.addComponent(drone_entity.getID(), renderable);

    auto physics = std::make_shared<PhysicsComponent>();
    btCollisionShape* shape = new btBoxShape(btVector3(size.x, size.y, size.z));
    btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(
        btQuaternion(0, 0, 0, 1),
        btVector3(position.x, position.y, position.z)
    ));
    btVector3 inertia(0,0,0);
    shape->calculateLocalInertia(mass, inertia);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, inertia);
    physics->body = new btRigidBody(rbInfo);
    physics->body->setDamping(linearDamping, angularDamping);
    manager.addComponent(drone_entity.getID(), physics);
}

void Drone::create_propellers(const glm::vec3& position, const glm::vec3& size) {
    float halfX = size.x;
    float halfZ = size.z;
    propellerOffsets[0] = glm::vec3(halfX, 0.25f, halfZ);
    propellerOffsets[1] = glm::vec3(-halfX, 0.25f, halfZ);
    propellerOffsets[2] = glm::vec3(-halfX, 0.25f, -halfZ);
    propellerOffsets[3] = glm::vec3(halfX, 0.25f, -halfZ);

    glm::vec3 color_array[4] = {
        glm::vec3(1.0f,0.0f,0.0f),
        glm::vec3(0.0f,1.0f,0.0f),
        glm::vec3(0.0f,0.0f,1.0f),
        glm::vec3(0.0f,1.0f,1.0f)
    };

    for (int i = 0; i < 4; ++i) {
        Entity prop;
        manager.addEntity(prop);

        auto propTransform = std::make_shared<TransformComponent>();
        propTransform->position = position + propellerOffsets[i];
        manager.addComponent(prop.getID(), propTransform);

        auto propRenderable = std::make_shared<RenderableComponent>(generateCube(glm::vec3(0.15f,0.02f,0.15f)));
        GLuint propTexture = createColorTexture(color_array[i], 256, 256);
        TextureComponent propTexComp{ propTexture, "texture_diffuse" };
        propRenderable->textures.push_back(propTexComp);
        propRenderable->meshes[0].textures.push_back(propTexComp);
        manager.addComponent(prop.getID(), propRenderable);

        propellerEntities[i] = prop;
    }

    rotorSpeeds.fill(0.0f);
}


void Drone::update(float dt) {
    auto physics = manager.getComponent<PhysicsComponent>(drone_entity.getID());
    auto baseTransform = manager.getComponent<TransformComponent>(drone_entity.getID());
    if (!physics || !physics->body || !baseTransform) return;

    btRigidBody* body = physics->body;
    btTransform transform = body->getWorldTransform();
    btQuaternion btRot = transform.getRotation();
    glm::quat orientation(btRot.getW(), btRot.getX(), btRot.getY(), btRot.getZ());

    float totalYawTorque = 0.0f;
    float forwardThrust = 0.0f;
    float rollThrust = 0.0f;

    for (int i = 0; i < 4; ++i) {
        currentRotorSpeeds[i] += (rotorSpeeds[i] - currentRotorSpeeds[i]) * dt / motorTau;
        float thrust = currentRotorSpeeds[i] * thrustCoefficient;

        if (i == 0 || i == 3) thrust -= forwardThrust;
        else thrust += forwardThrust;

        if (i == 0 || i == 1) thrust -= rollThrust;
        else thrust += rollThrust;

        thrust = std::max(thrust, 0.0f);

        glm::vec3 rotatedOffset = orientation * propellerOffsets[i];
        glm::vec3 worldPos = getPosition() + rotatedOffset;

        glm::vec3 localThrust(0.0f, thrust, 0.0f);
        glm::vec3 worldThrust = orientation * localThrust;

        body->applyForce(btVector3(worldThrust.x, worldThrust.y, worldThrust.z),
                         btVector3(rotatedOffset.x, rotatedOffset.y, rotatedOffset.z));

        float spinDir = (i == 0 || i == 3) ? 1.0f : -1.0f;
        totalYawTorque += currentRotorSpeeds[i] * torqueCoefficient * spinDir;

        auto propTransform = manager.getComponent<TransformComponent>(propellerEntities[i].getID());
        if (propTransform) {
            float angle = rotorSpeeds[i] * dt * 5.0f;
            glm::quat spin = glm::angleAxis(angle, glm::vec3(0.0f,1.0f,0.0f));
            propTransform->rotation = orientation * spin;
            propTransform->position = getPosition() + rotatedOffset;
        }
    }

    body->applyTorque(btVector3(0, totalYawTorque, 0));
}

glm::vec3 Drone::getPosition() const {
    auto physics = manager.getComponent<PhysicsComponent>(drone_entity.getID());
    if (!physics || !physics->body) return glm::vec3(0.0f);
    btVector3 pos = physics->body->getCenterOfMassPosition();
    return glm::vec3(pos.x(), pos.y(), pos.z());
}

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
