# udp_listener_sender.py
import socket
import json
import threading
import time

# ---------- Configuration ----------
RECV_IP = "127.0.0.1"
RECV_PORT = 12347

SEND_IP = "127.0.0.1"
SEND_PORT = 12348

# ---------- Listener ----------
def listener():
    recv_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    recv_sock.bind((RECV_IP, RECV_PORT))
    recv_sock.settimeout(1.0)
    print(f"Listening on {RECV_IP}:{RECV_PORT}...")
    try:
        while True:
            try:
                data, addr = recv_sock.recvfrom(1024)
                msg = data.decode(errors="ignore")
                print(len(msg), "bytes from", addr)
                try:
                    parsed = json.loads(msg)
                    print("Received JSON:", parsed)
                except json.JSONDecodeError:
                    print("Received (non-JSON):", msg)
            except socket.timeout:
                continue
    except KeyboardInterrupt:
        print("Listener shutting down.")
        recv_sock.close()

# ---------- Sender ----------
def sender():
    send_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    print(f"Sending to {SEND_IP}:{SEND_PORT}...")
    try:
        while True:
            message = {
                "type": "set_speeds",
                "front_right": 7.1,
                "front_left": 7.15,
                "back_left": 7.1,
                "back_right": 7.04
            }
            send_sock.sendto(json.dumps(message).encode(), (SEND_IP, SEND_PORT))
            time.sleep(1.0)  # send every second
    except KeyboardInterrupt:
        print("Sender shutting down.")
        send_sock.close()

# ---------- Run both threads ----------
if __name__ == "__main__":
    t1 = threading.Thread(target=listener, daemon=True)
    t2 = threading.Thread(target=sender, daemon=True)
    t1.start()
    t2.start()

    try:
        while True:
            time.sleep(0.1)
    except KeyboardInterrupt:
        print("Shutting down main thread.")
