import socket

UDP_IP = "127.0.0.1"
UDP_PORT = 12346

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))
sock.settimeout(1.0)  # seconds

print("Listening for messages...")
try:
    while True:
        try:
            data, addr = sock.recvfrom(1024)
            print(f"Received message: {data.decode()} from {addr}")
        except socket.timeout:
            continue  # Timeout occurred; loop and check again
except KeyboardInterrupt:
    print("Exiting...")
    sock.close()
