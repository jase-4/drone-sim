#ifndef MAGNETOMETER_SENSOR_H
#define MAGNETOMETER_SENSOR_H

#include "sensor.h"
#include "../drone/mqtt.h"
#include "sensor_utils.h"  // Include the utility class

#include <glm/glm.hpp>
#include <boost/asio.hpp>
#include <sstream>

class MagnetometerSensor : public Sensor<MagnetometerSensor> {
public:
    MagnetometerSensor(boost::asio::io_context& io, std::chrono::milliseconds updateRate = std::chrono::milliseconds(100))
        : Sensor(io, updateRate) {
        mqtt_publish_path = "mag/data";
    }

private:
    glm::vec3 magneticField;
    float noiseLevel = 0.01f;  // Noise level for the magnetic field data

    bool running = false;

    void update(float /*dt*/) override {
        // Simulate magnetic field (e.g., Earth's magnetic field)
        magneticField = glm::vec3(0.1f, 0.0f, 1.0f);

        // Add noise using SensorUtils to simulate sensor errors
        magneticField += SensorUtils::generateRandomNoise(noiseLevel);
    }

    // void publishData() override {
    //     std::ostringstream ss;
    //     ss << "{"
    //        << "\"mag_field\":[" << magneticField.x << "," << magneticField.y << "," << magneticField.z << "]"
    //        << "}";

    //     mqtt_.publish("magnetometer/data", ss.str());  // Use the MQTT reference from base class
    // }
};

#endif // MAGNETOMETER_SENSOR_H
