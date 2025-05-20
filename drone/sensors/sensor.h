#ifndef SENSOR_H
#define SENSOR_H

#include "..\drone\mqtt.h"
#include <boost/asio.hpp>
#include <chrono>
#include <memory>
#include <string>

// Base class for all sensors
class SensorBase {
    public:
        virtual ~SensorBase() = default;
        virtual void start() = 0;
        virtual void stop() = 0;
        virtual void update(float dt) = 0;  // For the sensor update logic
        //virtual void publishData() = 0;    // For the sensor data publishing logic
    };
    

template <typename SensorType>
class Sensor : public SensorBase {
public:
    // Constructor accepts MQTT reference, boost::asio::io_context, and update rate
    Sensor(boost::asio::io_context& io, std::chrono::milliseconds updateRate = std::chrono::milliseconds(100))
        : timer_(io), updateRate_(updateRate), running(false) {}

    // Override start and stop for the sensor
    void start() override {
        running = true;
        last_ = std::chrono::steady_clock::now();
        tick();
    }

    void stop() override {
        running = false;
        boost::system::error_code ec;
        timer_.cancel(ec);
    }
    std::string  mqtt_publish_path;

protected:

   
    void tick() {
        if (!running) return;

        auto now = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(now - last_).count();
        last_ = now;

        update(dt);  // Specific to each sensor type
       // publishData();  // Publish sensor data, specific to each sensor

        timer_.expires_after(updateRate_);
        timer_.async_wait([this](const boost::system::error_code& ec) {
            if (!ec) tick();
        });
    }

    virtual void update(float dt) override = 0;  // Update logic specific to the sensor
    //virtual void publishData() override = 0;  // Publish data specific to the sensor

   // MQTT& mqtt_;  // Shared MQTT reference, no type conflict

private:
    bool running;
    std::chrono::steady_clock::time_point last_;
    boost::asio::steady_timer timer_;
    std::chrono::milliseconds updateRate_; // Update rate for the sensor
};

#endif