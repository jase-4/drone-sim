#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <mutex>
#include <queue>
#include <boost/asio.hpp>

class Communicator {
public:

static Communicator& getInstance() {
    static Communicator instance; // thread-safe singleton
    return instance;
}

    Communicator(const Communicator&) = delete;
    Communicator& operator=(const Communicator&) = delete;
  

    using Callback = std::function<void(const std::string&)>;

    void subscribe(const std::string& topic, Callback cb);
    void publish(const std::string& topic, const std::string& message);
    void start();
    void stop();

private:
    void run();
    void sendMessage(const std::string& message);

    std::mutex mutex_;
    Communicator() : io_service_(), socket_(io_service_) {}
    ~Communicator(){
        stop();
    };
    
    std::unordered_map<std::string, std::vector<Callback>> subscribers_;
    std::queue<std::pair<std::string, std::string>> messageQueue_;
    
    std::thread thread_;
    std::atomic<bool> running{false};
    
    boost::asio::io_service io_service_;  // Boost Asio io_service
    boost::asio::ip::udp::socket socket_; // UDP socket
    boost::asio::ip::udp::endpoint remote_endpoint_;  // Remote endpoint (flight controller)

    // Example of a remote address and port (change as needed)
    const std::string remote_address_ = "127.0.0.1";  // Localhost for testing
    const unsigned short remote_port_ = 12345;         // Flight 
};

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
            sendMessage(msg);  // Send message over UDP
            localQueue.pop();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}


#endif // COMMUNICATOR_H
