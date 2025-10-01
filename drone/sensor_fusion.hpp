#ifndef SENSOR_FUSION_H
#define SENSOR_FUSION_H

#include "sensors/sensor.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <memory>
#include <sstream>

#include "sensors/imu.h"
#include "sensors/gps.h"

class SensorFusion {
public:
    void setSensors(const std::vector<std::unique_ptr<SensorBase>>& sensorReferences);
    void fuse(float dt);
    std::ostringstream publishData();

private:
    glm::vec3 fusedAcceleration{};
    glm::quat fusedOrientation{};

    glm::vec3 fusedPosition{};
    glm::vec3 fusedVelocity{};
    glm::vec3 imuAcceleration{};
    glm::quat imuOrientation{};
    glm::vec3 gpsPosition{};
    glm::vec3 gpsVelocity{};
    float pressure{};
};

#endif // SENSOR_FUSION_H
