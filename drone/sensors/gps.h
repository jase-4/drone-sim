#ifndef GPS_SENSOR_H
#define GPS_SENSOR_H

#include "sensor.h"
#include "../drone/mqtt.h"
#include "sensor_utils.h"  // Include the utility class

#include <glm/glm.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <sstream>
#include <cstdlib>

class GPSSensor : public Sensor<GPSSensor> {
public:
    // Initialize position and velocity
    GPSSensor(boost::asio::io_context& io, Drone& droneRef,   std::chrono::milliseconds updateRate = std::chrono::milliseconds(100))
        : Sensor(io, updateRate),  drone_(droneRef) {
        mqtt_publish_path = "gps";
    }

    void update(float dt) override;
   // void publishData() override;
    glm::vec3 position;  // lat, lon, alt
    glm::vec3 velocity;  // x, y, z m/s

private:
   // std::string toJSON();

   Drone& drone_;

    float noiseLevel = 0.01f;  // Noise level for position and velocity
    bool running = false;
};

void GPSSensor::update(float dt) {
    // Update position and velocity (this is just an example)
    position = drone_.getPosition(); // Assuming drone has a getPosition() method for lat, lon, alt
    velocity = drone_.getVelocity(); // Assuming drone has a getVelocity() method for speed in x, y, z


    // Add noise using SensorUtils for position and velocity
    position += SensorUtils::generateRandomNoise(noiseLevel);
    velocity += SensorUtils::generateRandomNoise(noiseLevel) * 0.1f;  // Less noise for velocity
}

// void GPSSensor::publishData() {
//     std::ostringstream ss;
//     ss << "{"
//        << "\"type\":\"gps\","
//        << "\"position\":[" << position.x << "," << position.y << "," << position.z << "],"
//        << "\"velocity\":[" << velocity.x << "," << velocity.y << "," << velocity.z << "]"
//        << "}";

//     mqtt_.publish("gps/data", ss.str());  // Use the MQTT reference from base class
// }

#endif // GPS_SENSOR_H
