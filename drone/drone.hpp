#ifndef DRONE_HPP
#define DRONE_HPP

#include "entity/entity_manager.hpp"
#include "renderer/render_components.hpp"
#include "physics_component.hpp"
#include "renderer/basic_shapes.h"
#include "sensors/sensor.h"
#include "mqtt.hpp"
#include "asyc_udp.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <array>
#include <vector>
#include <boost/asio/io_context.hpp>

class Drone {
public:
    Drone(const glm::vec3& position, const glm::vec3& size, float mass);

    void create_body(const glm::vec3& position, const glm::vec3& size, float mass);
    void create_propellers(const glm::vec3& position, const glm::vec3& size);

    void update(float dt);  

    void setRotorSpeed(int index, float speed);
    void setFrontRightRotorSpeed(float speed);
    void setFrontLeftRotorSpeed(float speed);
    void setBackRightRotorSpeed(float speed);
    void setBackLeftRotorSpeed(float speed);

    glm::vec3 getPosition() const;
    glm::quat getOrientation() const;
    glm::vec3 getVelocity() const;
    glm::vec3 getAngularVelocity() const;

    Entity drone_entity;
    std::array<float, 4> rotorSpeeds;

private:
    EntityManager& manager = EntityManager::getInstance();
    float linearDamping = 0.05f;
    float angularDamping = 0.25f;

    float motorTau = 0.07f;
    std::array<float, 4> currentRotorSpeeds = {0.0f, 0.0f, 0.0f, 0.0f};

    std::array<glm::vec3, 4> propellerOffsets;
    std::array<Entity, 4> propellerEntities;

    float thrustCoefficient = 3.68f / 4.0f;
    float torqueCoefficient = 0.003f;
};




#endif // DRONE_H
