#ifndef IMU_SENSOR_H
#define IMU_SENSOR_H

#include "sensor.h"
#include "../drone/mqtt.hpp"
#include "../drone/drone.hpp" 
#include "sensor_utils.hpp"  

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <sstream>
#include <chrono>

class IMUSensor : public Sensor<IMUSensor> {
public:
    IMUSensor(boost::asio::io_context& io, Drone& droneRef, std::chrono::milliseconds updateRate = std::chrono::milliseconds(100))
        : Sensor(io, updateRate),
          drone_(droneRef),
          acceleration_(0.0f),
          angularVelocity_(0.0f),
          orientation_(1.0f, 0.0f, 0.0f, 0.0f),
          prevVelocity_(0.0f),
          prevOrientation_(1.0f, 0.0f, 0.0f, 0.0f)
    {
        mqtt_publish_path = "imu";
    }

    void update(float dt) override {
      
       currentVelocity = drone_.getVelocity();
        glm::quat currentOrientation = drone_.getOrientation();

    
        acceleration_ = (currentVelocity - prevVelocity_) / dt;
        prevVelocity_ = currentVelocity;

       
        angularVelocity_ = drone_.getAngularVelocity();  
        orientation_ = currentOrientation;
        prevOrientation_ = currentOrientation;


        acceleration_ += SensorUtils::generateRandomNoise(noiseLevel_);
        angularVelocity_ += SensorUtils::generateRandomNoise(noiseLevel_);
    }

    
    glm::vec3 acceleration_;
    glm::vec3 angularVelocity_;
    glm::quat orientation_;

    glm::vec3 currentVelocity;

    glm::vec3 prevVelocity_;
    glm::quat prevOrientation_;

private:
    Drone& drone_;


    float noiseLevel_ = 0.02f;  
};

#endif // IMU_SENSOR_H


    // void publishData() override {
    //     std::ostringstream ss;
    //     ss << "{"
    //        << "\"type\":\"imu\","
    //        << "\"accel\":[" << acceleration_.x << "," << acceleration_.y << "," << acceleration_.z << "],"
    //        << "\"gyro\":[" << angularVelocity_.x << "," << angularVelocity_.y << "," << angularVelocity_.z << "],"
    //        << "\"quat\":[" << orientation_.w << "," << orientation_.x << "," << orientation_.y << "," << orientation_.z << "]"
    //        << "}";

    //     mqtt_.publish(mqtt_publish_path, ss.str());
    // }