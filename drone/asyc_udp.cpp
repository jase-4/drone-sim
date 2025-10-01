#include "asyc_udp.hpp"
#include <utility>

AsyncUDP::AsyncUDP(boost::asio::io_context& io_context, uint16_t listen_port, uint16_t s_port, MQTT& mqtt)
    : socket_(io_context, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), listen_port)),
      mqtt_(mqtt), send_port(s_port)
{
  
    // mqtt_.subscribe("imu/data", [this](const std::string& msg) {
    //     send_udp_message((std::move(msg)));
    // });

 
    mqtt_.subscribe("sensor/fusion", [this](const std::string& msg) {
        send_udp_message((std::move(msg)));
    });




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

       
        mqtt_.publish("udp/inbound", message); 
      
        start_receive();
    } else {
        std::cerr << "UDP receive error: " << error.message() << "\n";
    }
}

void AsyncUDP::send_udp_message(const std::string& message) {
    auto msg = std::make_shared<std::string>(message);  

    boost::asio::ip::udp::endpoint target(
        boost::asio::ip::address::from_string("127.0.0.1"), send_port
    );

    socket_.async_send_to(
        boost::asio::buffer(*msg), target,
        [msg](const boost::system::error_code& ec, std::size_t bytes_transferred) {
            if (ec) {
                std::cerr << "UDP send failed: " << ec.message() << "\n";
            }
          
        }
    );
}


