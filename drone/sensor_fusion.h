#ifndef SENSOR_FUSION_H
#define SENSOR_FUSION_H

#include "sensors/sensor.h"
#include <glm/glm.hpp>
#include <chrono>
#include <random>

#include "sensors/imu.h"
#include "sensors/gps.h"

class SensorFusion {
    public:
        // Pass sensors by reference or pointer
        void setSensors(const std::vector<std::unique_ptr<SensorBase>>& sensorReferences) {
            for (auto& sensor : sensorReferences) {
                if (auto* gps = dynamic_cast<GPSSensor*>(sensor.get())) {
                    gpsPosition = gps->position;
                    gpsVelocity = gps->velocity;
                }
                else if (auto* imu = dynamic_cast<IMUSensor*>(sensor.get())) {
                    imuAcceleration = imu->acceleration_;
                    imuOrientation = imu->orientation_;
                }
                // Add other sensors here in a similar way
            }
        }
    
        void fuse(float dt) {
            glm::vec3 imuVelocity = imuAcceleration * dt;
            glm::vec3 imuPosition = imuVelocity * dt;

            // Now you have an estimated position from IMU (imuPosition) and one from GPS (gpsPosition)
            // Fuse them (e.g., weighted average or Kalman Filter)

            float gpsWeight = 0.8f;
            float imuWeight = 0.2f;

            fusedPosition = gpsPosition;
           // fusedPosition = gpsPosition * gpsWeight + imuPosition * imuWeight;
           fusedVelocity = gpsVelocity;
           //fusedVelocity = gpsVelocity * gpsWeight + imuVelocity * imuWeight;

            // Orientation comes from IMU (gyro + accel)
            fusedOrientation = imuOrientation;
            fusedAcceleration = imuAcceleration;
        }

        std::ostringstream  publishData() {
            // Create a stringstream to format data
            std::ostringstream ss;
            ss << "{"
               << "\"type\":\"sensor_fusion\","
               << "\"fused_position\":[" 
               << fusedPosition.x << "," << fusedPosition.y << "," << fusedPosition.z << "],"
               << "\"fused_velocity\":[" 
               << fusedVelocity.x << "," << fusedVelocity.y << "," << fusedVelocity.z << "],"
               << "\"fused_acceleration\":[" 
               << fusedAcceleration.x << "," << fusedAcceleration.y << "," << fusedAcceleration.z << "],"
               << "\"fused_orientation\":[" 
               << fusedOrientation.w << "," << fusedOrientation.x << "," << fusedOrientation.y << "," << fusedOrientation.z << "]"
               << "}";
            return ss;
            // Publish the JSON data via MQTT (or another protocol)
           // mqtt_.publish("sensor/fusion", ss.str());
        }
      
    private:
    glm::vec3 fusedAcceleration;
    glm::quat fusedOrientation;

    glm::vec3 fusedPosition;
    glm::vec3 fusedVelocity;
    glm::vec3 imuAcceleration;
    glm::quat imuOrientation;
    glm::vec3 gpsPosition;
    glm::vec3 gpsVelocity;
    float pressure;

  
};

#endif // SENSOR_FUSION_H
