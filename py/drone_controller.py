


# drone_controller.py
import socket, json, time
import numpy as np
# from config import *
from helpers import *
from pid import PID

MASS = 1.5                # kg
GRAVITY = 9.81            # m/s^2
NUM_MOTORS = 4
THRUST_COEFF = 0.92       # N per rotor unit
SAFETY_MARGIN = 1.10      # 10% headroom
HOVER_THRUST = 4.1        # motor units per motor
RAMP_FACTOR = 0.02        # target smoothing
MAX_ANGLE_RAD = math.radians(10)
UDP_IP = "127.0.0.1"
UDP_PORT = 12346
TARGET_IP = "127.0.0.1"
TARGET_PORT = 12345


class DroneController:
    def __init__(self):
        # Networking
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock.bind((UDP_IP, UDP_PORT))
        self.sock.settimeout(1.0)
        time.sleep(2.5)  # wait for socket to be ready
        self.send_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        # PIDs
        self.alt_pid = PID(kp=0.6, ki=0.002, kd=0.51,
                           out_min=-3.0, out_max=3.0,
                           i_min=-2.0, i_max=2.0,
                           deriv_filter_tau=0.02)
        self.pos_pid_x = PID(kp=0.5, ki=0.012, kd=0.81)
        self.pos_pid_z = PID(kp=0.5, ki=0.012, kd=0.81)
        self.att_pid_roll = PID(kp=0.3315, ki=0.001, kd=0.3)
        self.att_pid_pitch = PID(kp=0.3315, ki=0.001, kd=0.3)
        self.yaw_pid = PID(kp=0.0, ki=0.0, kd=0.0)

        # Targets
        self.desired_pos = np.array([15.0, 20.0, -25.0])
        self.target_pos = np.array([0, 0.5, 0.0])
        self.ramp_factor = RAMP_FACTOR
        self.current_yaw_target = 0.0
        self.hover_thrust = HOVER_THRUST

        # PID init
        self.last_time = time.time()
        self.initialized = False
        self.flag = True

    def run(self):
        print("Listening for messages...")
        try:
            while True:
                try:
                    data, _ = self.sock.recvfrom(1024)
                    msg = json.loads(data.decode())
                    if msg.get("type") != "sensor_fusion":
                        continue
                    print(msg)
                    self.update(msg)
                except socket.timeout:
                    continue
        except KeyboardInterrupt:
            print("Shutting down.")
            self.sock.close()

    def update(self, msg):
        now = time.time()
        dt = now - self.last_time
        self.last_time = now
        if dt <= 0.0:
            return

        pos = np.array(msg["fused_position"])
        raw_q = msg["fused_orientation"]
        q_opengl = rotate_quaternion_for_opengl(raw_q)
        roll, pitch, yaw = quaternion_to_euler(q_opengl)

        print(roll,pitch,yaw)


        if np.linalg.norm(pos - self.desired_pos) < 1.5 and self.flag:
            self.target_pos = pos
            self.desired_pos = np.array([-15.0, 2.0, 5.0])
            self.flag = False

        # Initialize PID last_meas on first reading
        if not self.initialized:
            self.alt_pid.last_meas = pos[1]
            self.pos_pid_x.last_meas = pos[0]
            self.pos_pid_z.last_meas = pos[2]
            self.initialized = True

        # Ramp target
        self.target_pos += (self.desired_pos - self.target_pos) * self.ramp_factor
        self.current_yaw_target += (0.0 - self.current_yaw_target) * self.ramp_factor

        # PID updates
        thrust_correction = self.alt_pid.update(setpoint=self.target_pos[1],
                                                meas=pos[1],
                                                dt=dt,
                                                deadband=0.03)
        thrust = max(0.0, min(10.5, self.hover_thrust + thrust_correction))

        desired_roll = self.pos_pid_x.update(setpoint=self.target_pos[0], meas=pos[0], dt=dt)
        desired_pitch = self.pos_pid_z.update(setpoint=self.target_pos[2], meas=pos[2], dt=dt)

        desired_roll = max(-MAX_ANGLE_RAD, min(MAX_ANGLE_RAD, desired_roll))
        desired_pitch = max(-MAX_ANGLE_RAD, min(MAX_ANGLE_RAD, desired_pitch))

        roll_cmd = self.att_pid_roll.update(desired_roll, meas=roll, dt=dt)
        pitch_cmd = self.att_pid_pitch.update(-desired_pitch, meas=yaw, dt=dt)
        yaw_cmd = self.yaw_pid.update(self.current_yaw_target - pitch, dt=dt)

        motor_speeds = mixer(thrust, pitch_cmd, roll_cmd, yaw_cmd)
        self.send_sock.sendto(json.dumps(motor_speeds).encode(), (TARGET_IP, TARGET_PORT))

        print(f"pos={pos}, target={self.target_pos}, thrust={thrust:.3f}, "
              f"roll_cmd={roll_cmd:.6f}, pitch_cmd={pitch_cmd:.6f}, yaw_cmd={yaw_cmd:.6f}")
