import socket

def send_udp_message(message, ip, port):
    # Create a UDP socket
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        # Send the message to the specified address and port
        sock.sendto(message.encode(), (ip, port))
        print(f"Sent message: {message} to {ip}:{port}")

if __name__ == "__main__":
    # Message to be sent
    message = "Hello from Python!"
    
    # UDP server IP and port (same as in the C++ example)
    server_ip = "127.0.0.1"
    server_port = 12345

    # Send the UDP message
    send_udp_message(message, server_ip, server_port)
