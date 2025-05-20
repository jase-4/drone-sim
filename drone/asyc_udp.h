#ifndef ASYNC_UDP_H
#define ASYNC_UDP_H

#include <boost/asio.hpp>
#include <array>
#include <iostream>
#include "MQTT.h"  // Include your MQTT interface

class AsyncUDP {
public:
    AsyncUDP(boost::asio::io_context& io_context, uint16_t listen_port, uint16_t s_port, MQTT& mqtt);

    void subscribe_sensor(std::string& publish_path){
        mqtt_.subscribe(publish_path, [this](const std::string& msg) {
            send_udp_message((std::move(msg)));
        });
    }

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

AsyncUDP::AsyncUDP(boost::asio::io_context& io_context, uint16_t listen_port, uint16_t s_port, MQTT& mqtt)
    : socket_(io_context, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), listosen_port)),
      mqtt_(mqtt), send_port(s_port)
{
    // // Forward all outgoing MQTT sensor data (e.g., IMU) to UDP
    // mqtt_.subscribe("imu/data", [this](const std::string& msg) {
    //     send_udp_message((std::move(msg)));
    // });

 
    mqtt_.subscribe("sensor/fusion", [this](const std::string& msg) {
        send_udp_message((std::move(msg)));
    });


   // send_udp_message("hey bro");

    start_receive();
}

void AsyncUDP::start_receive() {
    socket_.async_receive_from(
        boost::asio::buffer(recv_buffer_), remote_endpoint_,
        [this](const boost::system::error_code& error, std::size_t bytes_transferred) {
            handle_receive(error, bytes_transferred);
        });
}

void AsyncUDP::handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred) {
    if (!error && bytes_transferred > 0) {
        std::string message(recv_buffer_.data(), bytes_transferred);

        // Optional: Route incoming UDP message to MQTT topic
        mqtt_.publish("udp/inbound", message);  // Example topic
       // std::cout << "yahhhh\n";
        // Continue listening
        start_receive();
    } else {
        std::cerr << "UDP receive error: " << error.message() << "\n";
    }
}

void AsyncUDP::send_udp_message(const std::string& message) {
    auto msg = std::make_shared<std::string>(message);  // Keep alive!

    boost::asio::ip::udp::endpoint target(
        boost::asio::ip::address::from_string("127.0.0.1"), send_port
    );

    socket_.async_send_to(
        boost::asio::buffer(*msg), target,
        [msg](const boost::system::error_code& ec, std::size_t bytes_transferred) {
            if (ec) {
                std::cerr << "UDP send failed: " << ec.message() << "\n";
            }
            // msg automatically deleted after this lambda exits
        }
    );
}


#endif // ASYNC_UDP_H
