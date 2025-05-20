#ifndef RANGEFINDER_SENSOR_H
#define RANGEFINDER_SENSOR_H

#include "sensor.h"
#include "../drone/mqtt.h"
#include "sensor_utils.h"  // Include the utility class

#include <boost/asio.hpp>
#include <sstream>

class RangefinderSensor : public Sensor<RangefinderSensor> {
public:
    RangefinderSensor(boost::asio::io_context& io, std::chrono::milliseconds updateRate = std::chrono::milliseconds(100))
        : Sensor(io, updateRate) {
        mqtt_publish_path = "rf/data";
    }

    void update(float /*dt*/) override {
        // Simulate changes in distance (random fluctuation)
        distance = 10.0f - (rand() % 10) * 0.1f;  // Random fluctuation in distance

        // Add noise using SensorUtils to simulate sensor errors
        distance += SensorUtils::generateRandomNoiseFloat(noiseLevel);
    }

    // void publishData() override {
    //     std::ostringstream ss;
    //     ss << "{"
    //        << "\"distance\":" << distance
    //        << "}";

    //     mqtt_.publish("rangefinder/data", ss.str());  // Use the MQTT reference from base class
    // }

private:
    float distance;
    float noiseLevel = 0.05f;
};

#endif // RANGEFINDER_SENSOR_H
