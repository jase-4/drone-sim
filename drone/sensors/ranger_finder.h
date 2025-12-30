#ifndef RANGEFINDER_SENSOR_H
#define RANGEFINDER_SENSOR_H

#include "sensor.h"
#include "../drone/mqtt.hpp"
#include "../drone/drone.hpp" 
#include "sensor_utils.hpp" 

#include <boost/asio.hpp>
#include <sstream>

class RangefinderSensor : public Sensor<RangefinderSensor> {
public:
    RangefinderSensor(boost::asio::io_context& io, Drone& droneRef, std::chrono::milliseconds updateRate = std::chrono::milliseconds(100))
        : Sensor(io, updateRate), drone_(droneRef) {
        mqtt_publish_path = "rf/data";
    }

    void update(float /*dt*/) override {
      distance = 10.0f - (rand() % 10) * 0.1f;  

       
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
    Drone& drone_;
    float distance;
    float noiseLevel = 0.05f;
};

#endif // RANGEFINDER_SENSOR_H
