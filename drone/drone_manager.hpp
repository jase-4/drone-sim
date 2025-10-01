#ifndef DRONE_MANAGER_HPP
#define DRONE_MANAGER_HPP

#include "drone.hpp"
#include "sensor_fusion.hpp"
#include "sensors/imu.h"
#include "sensors/gps.h"

#include <memory>
#include <vector>
#include <thread>
#include <chrono>
#include <iostream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class DroneManager {
public:
    DroneManager(Drone& drone);
    ~DroneManager();

    void update(float dt);
    void updateSensors(float dt);
    void handleSensorData(float dt);
    void manageCommunication();

    template <typename SensorType, typename... Args>
    void addSensor(std::chrono::milliseconds updateRate, Args&&... args);

    void startSensors();
    void stopSensors();
    void publishData();
    void subscribe();

private:
    Drone& drone_; 
    SensorFusion sensorFusion_; 
    std::vector<std::unique_ptr<SensorBase>> sensors; 

    uint16_t send_port = 12346;
    uint16_t listen_port = 12345;

    boost::asio::io_context io_context_; 
    std::unique_ptr<boost::asio::io_context::work> work_; 

public:
    MQTT mqtt;
    AsyncUDP async_udp;
};


template <typename SensorType, typename... Args>
void DroneManager::addSensor(std::chrono::milliseconds updateRate, Args&&... args) {
    auto sensor = std::make_unique<SensorType>(
        io_context_, drone_, updateRate, std::forward<Args>(args)...);
    sensors.push_back(std::move(sensor));
}

#endif // DRONE_MANAGER_H
