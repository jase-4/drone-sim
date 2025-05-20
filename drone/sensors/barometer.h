#ifndef BAROMETER_SENSOR_H
#define BAROMETER_SENSOR_H

#include "sensor.h"
#include "../drone/mqtt.h"
#include "sensor_utils.h"  // Include the utility class

#include <boost/asio.hpp>
#include <sstream>

class BarometerSensor : public Sensor<BarometerSensor> {
public:
    BarometerSensor(boost::asio::io_context& io,  std::chrono::milliseconds updateRate = std::chrono::milliseconds(100))
        : Sensor(io, updateRate), pressure(seaLevelPressure) {
        mqtt_publish_path = "baro/data";
    }

    void update(float /*dt*/) override {
        // Simulate barometer pressure (e.g., sea level pressure)
        pressure = seaLevelPressure + static_cast<float>(rand() % 10);  // Random fluctuation in pressure

        // Add noise using SensorUtils to simulate sensor errors
        pressure += SensorUtils::generateRandomNoiseFloat(noiseLevel);
    }

    // void publishData() override {
    //     std::ostringstream ss;
    //     ss << "{"
    //        << "\"pressure\":" << pressure
    //        << "}";

    //     mqtt_.publish("barometer/data", ss.str());  // Use the MQTT reference from base class
    // }

private:
    float pressure;
    float noiseLevel = 0.02f;
    float seaLevelPressure = 101325.0f;  // Standard sea level pressure in Pascals
};

#endif // BAROMETER_SENSOR_H
