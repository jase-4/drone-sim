#ifndef COMMUNICATOR_HPP
#define COMMUNICATOR_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <mutex>
#include <queue>
#include <boost/asio.hpp>
#include <thread>
#include <atomic>

class Communicator {
public:
    static Communicator& getInstance();
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
    Communicator();
    ~Communicator();
    std::unordered_map<std::string, std::vector<Callback>> subscribers_;
    std::queue<std::pair<std::string, std::string>> messageQueue_;
    std::thread thread_;
    std::atomic<bool> running;
    boost::asio::io_service io_service_;
    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::udp::endpoint remote_endpoint_;
    const std::string remote_address_ = "127.0.0.1";
    const unsigned short remote_port_ = 12345;
};

#endif // COMMUNICATOR_HPP
