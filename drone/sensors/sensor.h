#ifndef SENSOR_H
#define SENSOR_H

#include "..\drone\mqtt.hpp"
#include <boost/asio.hpp>
#include <chrono>
#include <memory>
#include <string>
#include <atomic>

extern std::atomic<uint64_t> message_count;
// Base class for all sensors
class SensorBase {
    public:
        virtual ~SensorBase() = default;
        virtual void start() = 0;
        virtual void stop() = 0;
        virtual void update(float dt) = 0;  
      
    };
    

template <typename SensorType>
class Sensor : public SensorBase {
public:
   
    Sensor(boost::asio::io_context& io, std::chrono::milliseconds updateRate = std::chrono::milliseconds(100))
        : timer_(io), updateRate_(updateRate), running(false) {}

    void start() override {
        running = true;
        last_ = std::chrono::steady_clock::now();
        nextTick_ = last_ + updateRate_; 
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
        if (!running)
            return;

        auto now = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(now - last_).count();
        last_ = now;
        message_count++;
        update(dt);

        // Guarantee next tick at fixed schedule:
        nextTick_ += updateRate_;

        timer_.expires_at(nextTick_);
        timer_.async_wait([this](const boost::system::error_code& ec) {
            if (!ec)
                tick();
        });
    }

    virtual void update(float dt) override = 0; 


private:
    bool running;
    std::chrono::steady_clock::time_point last_;
    std::chrono::steady_clock::time_point nextTick_;
    boost::asio::steady_timer timer_;
    std::chrono::milliseconds updateRate_;
};

#endif