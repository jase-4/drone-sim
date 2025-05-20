#ifndef DRONE_MANAGER_H
#define DRONE_MANAGER_H

#include "drone.h"
#include "sensor_fusion.h"
#include <memory>
#include <vector>

#include "drone_manager.h"
#include "sensors/imu.h"
#include "sensors/gps.h"


class DroneManager {
public:
    DroneManager(Drone& drone);
    void update(float dt);
    void updateSensors(float dt);
    void handleSensorData(float dt);
    void manageCommunication();

    
    ~DroneManager() {
        // Ensure all sensors are stopped before destructing
        stopSensors();
        // Stop io_context to release resources
        io_context_.stop();
    }

    template <typename SensorType, typename... Args>
    void addSensor(std::chrono::milliseconds updateRate, Args&&... args) {
        auto sensor = std::make_unique<SensorType>(
            io_context_, drone_, updateRate, std::forward<Args>(args)...);
       // async_udp.subscribe_sensor(sensor->mqtt_publish_path);
        sensors.push_back(std::move(sensor));
    }

    // Start all sensors
    void startSensors() {
        for (auto& sensor : sensors) {
            sensor->start();
        }
        std::thread([this]() { io_context_.run(); }).detach();
    }

    // Stop all sensors
    void stopSensors() {
        for (auto& sensor : sensors) {
            sensor->stop();
        }
    }

    void publishData() {
            mqtt.publish("sensor/fusion", sensorFusion_.publishData().str());
        }

private:
    Drone& drone_;  // Reference to the drone
    SensorFusion sensorFusion_;  // Handles fusion and error injection
    std::vector<std::unique_ptr<SensorBase>> sensors;  // List of sensors

    uint16_t send_port = 12346;
    uint16_t listen_port = 12345;


    boost::asio::io_context io_context_;  // Internal io_context for handling sensor updates
    std::unique_ptr<boost::asio::io_context::work> work_; // Keeps io_context running
    public:
    MQTT mqtt;
    AsyncUDP async_udp;

};




// DroneManager::DroneManager(Drone& drone, boost::asio::io_context& io_context)
//     : drone_(drone), io_context_(io_context) {
   
//  }

 DroneManager::DroneManager(Drone& drone) 
  : io_context_(), drone_(drone),
    work_(std::make_unique<boost::asio::io_context::work>(io_context_)),
    mqtt(io_context_),
    async_udp(io_context_, listen_port, send_port, mqtt)
{

     // Initialize sensor fusion
     sensorFusion_ = SensorFusion();
    
     // Add sensors
 //     drone_.addSensor<IMUSensor>(std::chrono::milliseconds(10));  // Example sensor
 //     drone_.addSensor<GPSSensor>(std::chrono::milliseconds(100)); // Example sensor
   
}
void DroneManager::update(float dt){
    drone_.update(dt);
    updateSensors(dt);
    sensorFusion_.setSensors(sensors);
    sensorFusion_.fuse(dt);
    publishData();



}

void DroneManager::updateSensors(float dt) {
    // Fetch sensor data (GPS, IMU, etc.)
    // auto imuData = drone_.getSensorData<IMUSensor>();
    // auto gpsData = drone_.getSensorData<GPSSensor>();

    // Perform sensor fusion
    // auto fusedData = sensorFusion_.fuseSensorData(gpsData, imuData, dt);

    // // Apply errors (optional)
    // fusedData = sensorFusion_.applySensorError(fusedData, 0.05f);

    // Update drone's state with fused data
   // drone_.setPosition(fusedData);  // Assuming fused data is position
}

void DroneManager::handleSensorData(float dt) {
    // Example of how to handle sensor data update logic
    updateSensors(dt);

    // Send or process data as necessary
    //drone_.mqtt.publish("drone/telemetry", "sensor_data", fusedData);
}

#endif // DRONE_MANAGER_H
