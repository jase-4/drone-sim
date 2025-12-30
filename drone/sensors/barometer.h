#ifndef BAROMETER_SENSOR_H
#define BAROMETER_SENSOR_H

#include "sensor.h"
#include "../drone/mqtt.hpp"
#include "sensor_utils.hpp"  

#include <boost/asio.hpp>
#include <sstream>

class BarometerSensor : public Sensor<BarometerSensor> {
public:
    BarometerSensor(boost::asio::io_context& io,  Drone& droneRef, std::chrono::milliseconds updateRate = std::chrono::milliseconds(100))
        : Sensor(io, updateRate),  pressure(seaLevelPressure),  drone_(droneRef) {
        mqtt_publish_path = "baro/data";
    }

    void update(float /*dt*/) override {
       
        pressure = seaLevelPressure + static_cast<float>(rand() % 10); 
        pressure += SensorUtils::generateRandomNoiseFloat(noiseLevel);
    }

    // void publishData() override {
    //     std::ostringstream ss;
    //     ss << "{"
    //        << "\"pressure\":" << pressure
    //        << "}";

    //     mqtt_.publish("barometer/data", ss.str()); 
    // }

private:
      Drone& drone_;
    float pressure;
    float noiseLevel = 0.02f;
    float seaLevelPressure = 101325.0f; 
};

#endif // BAROMETER_SENSOR_H
