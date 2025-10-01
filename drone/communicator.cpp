#include "communicator.hpp"
#include <utility>

void Communicator::sendMessage(const std::string& message) {
    remote_endpoint_ = boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(remote_address_), remote_port_);
    socket_.send_to(boost::asio::buffer(message), remote_endpoint_);
}

void Communicator::subscribe(const std::string& topic, Callback cb) {
    std::lock_guard<std::mutex> lock(mutex_);
    subscribers_[topic].push_back(std::move(cb));
}

void Communicator::publish(const std::string& topic, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    messageQueue_.emplace(topic, message);
}

void Communicator::start() {
    running = true;
    thread_ = std::thread(&Communicator::run, this);
}

void Communicator::stop() {
    running = false;
    if (thread_.joinable()) thread_.join();
}

void Communicator::run() {
    socket_.open(boost::asio::ip::udp::v4());



    while (running) {
        std::queue<std::pair<std::string, std::string>> localQueue;

        {
            std::lock_guard<std::mutex> lock(mutex_);
            std::swap(localQueue, messageQueue_);
        }

        while (!localQueue.empty()) {
            auto& [topic, msg] = localQueue.front();
            if (subscribers_.count(topic)) {
                for (auto& cb : subscribers_[topic]) {
                    cb(msg);
                }
            }
            sendMessage(msg); 
            localQueue.pop();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
