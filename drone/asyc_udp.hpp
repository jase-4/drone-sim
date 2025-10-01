#ifndef ASYC_UDP_HPP
#define ASYC_UDP_HPP

#include <boost/asio.hpp>
#include <array>
#include <iostream>
#include "mqtt.hpp"

class MQTT;
class AsyncUDP {
public:
    AsyncUDP(boost::asio::io_context& io_context, uint16_t listen_port, uint16_t s_port, MQTT& mqtt);
    void subscribe_sensor(std::string& publish_path);
private:
    void start_receive();
    void handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred);
    void send_udp_message(const std::string& message);
    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::udp::endpoint remote_endpoint_;
    std::array<char, 1024> recv_buffer_;
    uint16_t send_port;
    MQTT& mqtt_;
};

#endif // ASYC_UDP_HPP
