#include "drone_manager.hpp"

DroneManager::DroneManager(Drone& drone)
    : io_context_(),
      drone_(drone),
      work_(std::make_unique<boost::asio::io_context::work>(io_context_)),
      mqtt(io_context_),
      async_udp(io_context_, listen_port, send_port, mqtt)
{
    // Initialize sensor fusion
    sensorFusion_ = SensorFusion();

   
    // addSensor<IMUSensor>(std::chrono::milliseconds(10));
    // addSensor<GPSSensor>(std::chrono::milliseconds(100));
}

DroneManager::~DroneManager() {
    stopSensors();
    io_context_.stop();
}

void DroneManager::update(float dt) {
    drone_.update(dt);
    updateSensors(dt);
    sensorFusion_.setSensors(sensors);
    sensorFusion_.fuse(dt);
    publishData();
}

void DroneManager::updateSensors(float dt) {
    
}

void DroneManager::handleSensorData(float dt) {
    updateSensors(dt);
}

void DroneManager::startSensors() {
    for (auto& sensor : sensors) {
        sensor->start();
    }
    std::thread([this]() { io_context_.run(); }).detach();
}

void DroneManager::stopSensors() {
    for (auto& sensor : sensors) {
        sensor->stop();
    }
}

void DroneManager::publishData() {
    mqtt.publish("sensor/fusion", sensorFusion_.publishData().str());
}

void DroneManager::subscribe() {
    mqtt.subscribe("udp/inbound", [this](const std::string& msg) {
        std::cout << "Received: " << msg << std::endl;
        try {
            auto data = json::parse(msg);
            if (data.contains("type") && data["type"] == "set_speeds") {
                if (data.contains("front_right"))  drone_.setFrontRightRotorSpeed(data["front_right"]);
                if (data.contains("front_left"))   drone_.setFrontLeftRotorSpeed(data["front_left"]);
                if (data.contains("back_left"))    drone_.setBackLeftRotorSpeed(data["back_left"]);
                if (data.contains("back_right"))   drone_.setBackRightRotorSpeed(data["back_right"]);
            }
        } catch (const std::exception& e) {
            std::cerr << "Failed to parse or handle JSON: " << e.what() << std::endl;
        }
    });
}
