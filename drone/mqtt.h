#ifndef MQTT_H
#define MQTT_H

#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <boost/asio.hpp>

class MQTT {
public:
    using Callback = std::function<void(const std::string&)>;

    explicit MQTT(boost::asio::io_context& io) : io_(io) {}

    void subscribe(const std::string& topic, Callback cb) {
        subscribers_[topic].push_back(std::move(cb));
    }

    void publish(const std::string& topic, const std::string& message) {
        boost::asio::post(io_, [this, topic, message]() {
            auto it = subscribers_.find(topic);
            if (it != subscribers_.end()) {
                for (auto& cb : it->second) {
                    cb(message);
                }
            }
        });
    }

private:
    boost::asio::io_context& io_;
    std::unordered_map<std::string, std::vector<Callback>> subscribers_;
};

#endif
